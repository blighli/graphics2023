#include <message.hpp>

#include "imgui_impl_window.h"
#include "imgui_impl_renderer.h"

static ImGuiWindowData *getBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGuiWindowData *)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

#ifdef HAS_SDL_IMPL
#include <SDL_syswm.h>
#include <window_sdl.h>
#include <input_sdl.hpp>
static const char *getClipboardText_SDL(void *)
{
    ImGuiWindowData *bd = getBackendData();
    if (bd->ClipboardTextData)
        SDL_free(bd->ClipboardTextData);
    bd->ClipboardTextData = SDL_GetClipboardText();
    return bd->ClipboardTextData;
}
static void setClipboardText_SDL(void *, const char *text)
{
    SDL_SetClipboardText(text);
}
static void setPlatformImeData_SDL(ImGuiViewport *, ImGuiPlatformImeData *data)
{
    if (data->WantVisible)
    {
        SDL_Rect r;
        r.x = (int)data->InputPos.x;
        r.y = (int)data->InputPos.y;
        r.w = 1;
        r.h = (int)data->InputLineHeight;
        SDL_SetTextInputRect(&r);
    }
}
#endif

ImGuiWindow::ImGuiWindow(Window &window, ImGuiRenderer &renderer)
    : m_windowHandle(&window), m_rendererHandle(std::make_unique<ImGuiRenderer>(renderer))
{
    ImGuiIO &io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

    ImGuiWindowData *bd = new ImGuiWindowData;
    io.BackendPlatformUserData = static_cast<void *>(bd);
    io.BackendPlatformName = "imgui_impl_window";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)

    // Set platform dependent data in viewport
    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    main_viewport->PlatformHandleRaw = nullptr;
    if (typeid(window).name() == typeid(SDLWindow).name())
    {
        m_windowBackendType = WINDOW_BACKEND_TYPE_SDL;
        m_inputHandle = new SDLInputSystem;

        io.GetClipboardTextFn = getClipboardText_SDL;
        io.SetClipboardTextFn = setClipboardText_SDL;
        io.ClipboardUserData = nullptr;
        io.SetPlatformImeDataFn = setPlatformImeData_SDL;

        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        if (SDL_GetWindowWMInfo(static_cast<SDL_Window *>(m_windowHandle->getWindowHandle()), &info))
        {
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
            main_viewport->PlatformHandleRaw = (void *)info.info.win.window;
#elif defined(__APPLE__) && defined(SDL_VIDEO_DRIVER_COCOA)
            main_viewport->PlatformHandleRaw = (void *)info.info.cocoa.window;
#endif
        }
    }
    m_rendererHandle->m_surface = m_windowHandle->createSurface(m_rendererHandle->m_renderContextHandle->getInstanceHandle());

    MessageHandler::getInstance().addWindowMouseFocusOnCallback([](std::any, std::any ptr)
                                                                { std::any_cast<ImGuiWindowData *>(ptr)->PendingMouseLeaveFrame = 0; },
                                                                bd);
    MessageHandler::getInstance().addWindowMouseFocusOffCallback([](std::any, std::any ptr)
                                                                 { std::any_cast<ImGuiWindowData *>(ptr)->PendingMouseLeaveFrame = ImGui::GetFrameCount() + 1; },
                                                                 bd);
    MessageHandler::getInstance().addWindowKeyboardFocusOnCallback([](std::any, std::any ptr)
                                                                   { std::any_cast<ImGuiIO *>(ptr)->AddFocusEvent(true); },
                                                                   &io);
    MessageHandler::getInstance().addWindowKeyboardFocusOffCallback([](std::any, std::any ptr)
                                                                    { std::any_cast<ImGuiIO *>(ptr)->AddFocusEvent(false); },
                                                                    bd);
    MessageHandler::getInstance().addMouseMotionCallback([](int32_t x, int32_t y, bool touch, std::any io_ptr)
                                                         {
        std::any_cast<ImGuiIO*>(io_ptr)->AddMouseSourceEvent(touch ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
        std::any_cast<ImGuiIO*>(io_ptr)->AddMousePosEvent(x, y); },
                                                         &io);
    MessageHandler::getInstance().addMouseWheelCallback([](int32_t wheelX, int32_t wheelY, bool touch, std::any io_ptr)
                                                        {
        float x = wheelX, y = wheelY;
#ifdef __EMSCRIPTEN__
            x /= 100.0f;
#endif
        std::any_cast<ImGuiIO*>(io_ptr)->AddMouseSourceEvent(touch ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
        std::any_cast<ImGuiIO*>(io_ptr)->AddMouseWheelEvent(x, y); },
                                                        &io);
    MessageHandler::getInstance().addMouseButtonDownCallback([&](eMouseKeyCode mouseKey, int32_t x, int32_t y, bool touch, std::any ptrs)
                                                             {
        auto real_ptrs = std::any_cast<std::pair<ImGuiIO*, ImGuiWindowData*>>(ptrs);
        int mouse_button = -1;
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_LB) { mouse_button = 0; }
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_RB) { mouse_button = 1; }
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_MB) { mouse_button = 2; }
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_X1B) { mouse_button = 3; }
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_X2B) { mouse_button = 4; }
        if (mouse_button == -1) return;
        real_ptrs.first->AddMouseSourceEvent(touch ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
        real_ptrs.first->AddMouseButtonEvent(mouse_button, true);
        real_ptrs.second->MouseButtonsDown |= (1 << mouse_button); },
                                                             std::pair{&io, bd});
    MessageHandler::getInstance().addMouseButtonUpCallback([&](eMouseKeyCode mouseKey, int32_t x, int32_t y, bool touch, std::any ptrs)
                                                           {
        auto real_ptrs = std::any_cast<std::pair<ImGuiIO*, ImGuiWindowData*>>(ptrs);
        int mouse_button = -1;
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_LB) { mouse_button = 0; }
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_RB) { mouse_button = 1; }
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_MB) { mouse_button = 2; }
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_X1B) { mouse_button = 3; }
        if (mouseKey == eMouseKeyCode::MOUSE_KEY_CODE_X2B) { mouse_button = 4; }
        if (mouse_button == -1) return;
        real_ptrs.first->AddMouseSourceEvent(touch ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
        real_ptrs.first->AddMouseButtonEvent(mouse_button, true);
        real_ptrs.second->MouseButtonsDown |= ~(1 << mouse_button); },
                                                           std::pair{&io, bd});
    MessageHandler::getInstance().addKeyDownCallback([&](eKeyCode key, std::any io_ptr)
                                                     { std::any_cast<ImGuiIO *>(io_ptr)->AddKeyEvent(keyCodeTranslatorBack(key), true); },
                                                     &io);
    MessageHandler::getInstance().addKeyUpCallback([&](eKeyCode key, std::any io_ptr)
                                                   { std::any_cast<ImGuiIO *>(io_ptr)->AddKeyEvent(keyCodeTranslatorBack(key), false); },
                                                   &io);
    MessageHandler::getInstance().addTextInputCallback([](const char *text, std::any io_ptr)
                                                       { std::any_cast<ImGuiIO *>(io_ptr)->AddInputCharactersUTF8(text); },
                                                       &io);
}

ImGuiWindow::~ImGuiWindow()
{
    ImGuiWindowData *bd = getBackendData();
    IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO &io = ImGui::GetIO();

    if (bd->ClipboardTextData)
    {
        if (m_windowBackendType == WINDOW_BACKEND_TYPE_SDL)
            SDL_free(bd->ClipboardTextData);
    }

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad);
    IM_DELETE(bd);

    m_rendererHandle.reset();
    delete m_inputHandle;
}

void ImGuiWindow::newFrame()
{
    ImGuiWindowData *bd = getBackendData();
    IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplSDL2_Init()?");
    ImGuiIO &io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    auto size = m_windowHandle->getSize();
    if (m_windowHandle->isMinimized())
        size = {0u, 0U};
    auto display_size = m_windowHandle->getDisplaySize();
    io.DisplaySize = ImVec2(static_cast<float>(size.first), static_cast<float>(size.second));
    if (io.DisplaySize.x > 0 && io.DisplaySize.y > 0)
        io.DisplayFramebufferScale = ImVec2(static_cast<float>(display_size.first) / io.DisplaySize.x, static_cast<float>(display_size.second) / io.DisplaySize.y);

    // Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
    // (Accept SDL_GetPerformanceCounter() not returning a monotonically increasing value. Happens in VMs and Emscripten, see #6189, #6114, #3644)
    static size_t frequency = (m_windowBackendType == WINDOW_BACKEND_TYPE_SDL) ? reinterpret_cast<SDLWindow *>(m_windowHandle)->getFrequency() : 1U;
    size_t current_time = m_windowHandle->getCurrentTime();
    if (current_time <= bd->Time)
        current_time = bd->Time + 1;
    io.DeltaTime = bd->Time > 0 ? static_cast<float>(static_cast<double>(current_time - bd->Time) / frequency) : static_cast<float>(1.f / 60.f);
    bd->Time = current_time;

    if (bd->PendingMouseLeaveFrame && bd->PendingMouseLeaveFrame >= ImGui::GetFrameCount() && bd->MouseButtonsDown == 0)
    {
        bd->PendingMouseLeaveFrame = 0;
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    }

    updateMouseData();
    updateMouseCursor();

    // // Update game controllers (if enabled and available)
    // updateGamePads();

    ImGui::NewFrame();
    ImGui::Render();
    if (!m_windowHandle->isMinimized())
        m_rendererHandle->render();
}

void ImGuiWindow::updateMouseData()
{
    ImGuiWindowData *bd = getBackendData();
    ImGuiIO &io = ImGui::GetIO();

    // We forward mouse input when hovered or captured (via SDL_MOUSEMOTION) or when focused (below)
#if INPUT_HAS_CAPTURE_AND_GLOBAL_MOUSE
    if (m_windowBackendType == WINDOW_BACKEND_TYPE_SDL)
    {
        // SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger other operations outside
        SDL_CaptureMouse((bd->MouseButtonsDown != 0) ? SDL_TRUE : SDL_FALSE);
    }
#endif
    if (m_windowHandle->isFocused())
    {
        // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
        if (io.WantSetMousePos)
            m_inputHandle->setCursorPos({io.MousePos.x, io.MousePos.y}, m_windowHandle->getWindowHandle());

        // (Optional) Fallback to provide mouse position when focused (SDL_MOUSEMOTION already provides this when hovered or captured)
        if (m_inputHandle->mouseCanUseGlobalState() && bd->MouseButtonsDown == 0)
        {
            auto cursorPos = m_inputHandle->getCursorPos(eCursorCoordinate::CURSOR_COORDINATE_SCREEN);
            auto pos = m_windowHandle->getPosition();
            io.AddMousePosEvent(static_cast<float>(cursorPos.first - pos.first), static_cast<float>(cursorPos.second - pos.second));
        }
    }
}

void ImGuiWindow::updateMouseCursor()
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;
    ImGuiWindowData *bd = getBackendData();

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (io.MouseDrawCursor)
        m_inputHandle->showCursor(false);
    if (!m_inputHandle->setCursorType(mouseCursorTranslator(imgui_cursor)))
        m_inputHandle->setCursorType(eMouseCursor::MOUSE_CURSOR_ARROW);
}

eMouseCursor ImGuiWindow::mouseCursorTranslator(ImGuiMouseCursor cursor)
{
    switch (cursor)
    {
        KEY_CODE_TRANS(ImGuiMouseCursor_Arrow, MOUSE_CURSOR_ARROW)
        KEY_CODE_TRANS(ImGuiMouseCursor_TextInput, MOUSE_CURSOR_TEXT_INPUT)
        KEY_CODE_TRANS(ImGuiMouseCursor_ResizeAll, MOUSE_CURSOR_RESIZE_ALL)
        KEY_CODE_TRANS(ImGuiMouseCursor_ResizeEW, MOUSE_CURSOR_RESIZE_EW)
        KEY_CODE_TRANS(ImGuiMouseCursor_ResizeNS, MOUSE_CURSOR_RESIZE_NS)
        KEY_CODE_TRANS(ImGuiMouseCursor_ResizeNESW, MOUSE_CURSOR_RESIZE_NESW)
        KEY_CODE_TRANS(ImGuiMouseCursor_ResizeNWSE, MOUSE_CURSOR_RESIZE_NWSE)
        KEY_CODE_TRANS(ImGuiMouseCursor_Hand, MOUSE_CURSOR_HAND)
        KEY_CODE_TRANS(ImGuiMouseCursor_NotAllowed, MOUSE_CURSOR_NOT_ALLOWED)
    default:
        return eMouseCursor::MOUSE_CURSOR_HIDE;
    }
}

ImGuiKey ImGuiWindow::keyCodeTranslatorBack(eKeyCode keyCode)
{
    switch (keyCode)
    {
        KEY_CODE_TRANS(KEY_CODE_Tab, ImGuiKey_Tab)
        KEY_CODE_TRANS(KEY_CODE_Left, ImGuiKey_LeftArrow)
        KEY_CODE_TRANS(KEY_CODE_Right, ImGuiKey_RightArrow)
        KEY_CODE_TRANS(KEY_CODE_Up, ImGuiKey_UpArrow)
        KEY_CODE_TRANS(KEY_CODE_Down, ImGuiKey_DownArrow)
        KEY_CODE_TRANS(KEY_CODE_PageUp, ImGuiKey_PageUp)
        KEY_CODE_TRANS(KEY_CODE_PageDown, ImGuiKey_PageDown)
        KEY_CODE_TRANS(KEY_CODE_Home, ImGuiKey_Home)
        KEY_CODE_TRANS(KEY_CODE_End, ImGuiKey_End)
        KEY_CODE_TRANS(KEY_CODE_Insert, ImGuiKey_Insert)
        KEY_CODE_TRANS(KEY_CODE_Del, ImGuiKey_Delete)
        KEY_CODE_TRANS(KEY_CODE_Backspace, ImGuiKey_Backspace)
        KEY_CODE_TRANS(KEY_CODE_SpaceBar, ImGuiKey_Space)
        KEY_CODE_TRANS(KEY_CODE_Enter, ImGuiKey_Enter)
        KEY_CODE_TRANS(KEY_CODE_Esc, ImGuiKey_Escape)
        KEY_CODE_TRANS(KEY_CODE_Quote, ImGuiKey_Apostrophe)
        KEY_CODE_TRANS(KEY_CODE_Comma, ImGuiKey_Comma)
        KEY_CODE_TRANS(KEY_CODE_Minus, ImGuiKey_Minus)
        KEY_CODE_TRANS(KEY_CODE_Dot, ImGuiKey_Period)
        KEY_CODE_TRANS(KEY_CODE_Slash, ImGuiKey_Slash)
        KEY_CODE_TRANS(KEY_CODE_Semicolon, ImGuiKey_Semicolon)
        KEY_CODE_TRANS(KEY_CODE_Plus, ImGuiKey_Equal)
        KEY_CODE_TRANS(KEY_CODE_LBracket, ImGuiKey_LeftBracket)
        KEY_CODE_TRANS(KEY_CODE_Backslash, ImGuiKey_Backslash)
        KEY_CODE_TRANS(KEY_CODE_RBracket, ImGuiKey_RightBracket)
        KEY_CODE_TRANS(KEY_CODE_Wave, ImGuiKey_GraveAccent)
        KEY_CODE_TRANS(KEY_CODE_CapsLock, ImGuiKey_CapsLock)
        KEY_CODE_TRANS(KEY_CODE_Scrolllock, ImGuiKey_ScrollLock)
        KEY_CODE_TRANS(KEY_CODE_Numlock, ImGuiKey_NumLock)
        KEY_CODE_TRANS(KEY_CODE_Printscreen, ImGuiKey_PrintScreen)
        KEY_CODE_TRANS(KEY_CODE_Pause, ImGuiKey_Pause)
        KEY_CODE_TRANS(KEY_CODE_Numpad0, ImGuiKey_Keypad0)
        KEY_CODE_TRANS(KEY_CODE_Numpad1, ImGuiKey_Keypad1)
        KEY_CODE_TRANS(KEY_CODE_Numpad2, ImGuiKey_Keypad2)
        KEY_CODE_TRANS(KEY_CODE_Numpad3, ImGuiKey_Keypad3)
        KEY_CODE_TRANS(KEY_CODE_Numpad4, ImGuiKey_Keypad4)
        KEY_CODE_TRANS(KEY_CODE_Numpad5, ImGuiKey_Keypad5)
        KEY_CODE_TRANS(KEY_CODE_Numpad6, ImGuiKey_Keypad6)
        KEY_CODE_TRANS(KEY_CODE_Numpad7, ImGuiKey_Keypad7)
        KEY_CODE_TRANS(KEY_CODE_Numpad8, ImGuiKey_Keypad8)
        KEY_CODE_TRANS(KEY_CODE_Numpad9, ImGuiKey_Keypad9)
        KEY_CODE_TRANS(KEY_CODE_Decimal, ImGuiKey_KeypadDecimal)
        KEY_CODE_TRANS(KEY_CODE_Divide, ImGuiKey_KeypadDivide)
        KEY_CODE_TRANS(KEY_CODE_Multiply, ImGuiKey_KeypadMultiply)
        KEY_CODE_TRANS(KEY_CODE_Subtract, ImGuiKey_KeypadSubtract)
        KEY_CODE_TRANS(KEY_CODE_Add, ImGuiKey_KeypadAdd)
        KEY_CODE_TRANS(KEY_CODE_NumpadEnter, ImGuiKey_KeypadEnter)
        KEY_CODE_TRANS(KEY_CODE_NumpadEqual, ImGuiKey_KeypadEqual)
        KEY_CODE_TRANS(KEY_CODE_LCtrl, ImGuiKey_LeftCtrl)
        KEY_CODE_TRANS(KEY_CODE_LShift, ImGuiKey_LeftShift)
        KEY_CODE_TRANS(KEY_CODE_LAlt, ImGuiKey_LeftAlt)
        KEY_CODE_TRANS(KEY_CODE_LGui, ImGuiKey_LeftSuper)
        KEY_CODE_TRANS(KEY_CODE_RCtrl, ImGuiKey_RightCtrl)
        KEY_CODE_TRANS(KEY_CODE_RShift, ImGuiKey_RightShift)
        KEY_CODE_TRANS(KEY_CODE_RAlt, ImGuiKey_RightAlt)
        KEY_CODE_TRANS(KEY_CODE_RGui, ImGuiKey_RightSuper)
        KEY_CODE_TRANS(KEY_CODE_App, ImGuiKey_Menu)
        KEY_CODE_TRANS(KEY_CODE_0, ImGuiKey_0)
        KEY_CODE_TRANS(KEY_CODE_1, ImGuiKey_1)
        KEY_CODE_TRANS(KEY_CODE_2, ImGuiKey_2)
        KEY_CODE_TRANS(KEY_CODE_3, ImGuiKey_3)
        KEY_CODE_TRANS(KEY_CODE_4, ImGuiKey_4)
        KEY_CODE_TRANS(KEY_CODE_5, ImGuiKey_5)
        KEY_CODE_TRANS(KEY_CODE_6, ImGuiKey_6)
        KEY_CODE_TRANS(KEY_CODE_7, ImGuiKey_7)
        KEY_CODE_TRANS(KEY_CODE_8, ImGuiKey_8)
        KEY_CODE_TRANS(KEY_CODE_9, ImGuiKey_9)
        KEY_CODE_TRANS(KEY_CODE_A, ImGuiKey_A)
        KEY_CODE_TRANS(KEY_CODE_B, ImGuiKey_B)
        KEY_CODE_TRANS(KEY_CODE_C, ImGuiKey_C)
        KEY_CODE_TRANS(KEY_CODE_D, ImGuiKey_D)
        KEY_CODE_TRANS(KEY_CODE_E, ImGuiKey_E)
        KEY_CODE_TRANS(KEY_CODE_F, ImGuiKey_F)
        KEY_CODE_TRANS(KEY_CODE_G, ImGuiKey_G)
        KEY_CODE_TRANS(KEY_CODE_H, ImGuiKey_H)
        KEY_CODE_TRANS(KEY_CODE_I, ImGuiKey_I)
        KEY_CODE_TRANS(KEY_CODE_J, ImGuiKey_J)
        KEY_CODE_TRANS(KEY_CODE_K, ImGuiKey_K)
        KEY_CODE_TRANS(KEY_CODE_L, ImGuiKey_L)
        KEY_CODE_TRANS(KEY_CODE_M, ImGuiKey_M)
        KEY_CODE_TRANS(KEY_CODE_N, ImGuiKey_N)
        KEY_CODE_TRANS(KEY_CODE_O, ImGuiKey_O)
        KEY_CODE_TRANS(KEY_CODE_P, ImGuiKey_P)
        KEY_CODE_TRANS(KEY_CODE_Q, ImGuiKey_Q)
        KEY_CODE_TRANS(KEY_CODE_R, ImGuiKey_R)
        KEY_CODE_TRANS(KEY_CODE_S, ImGuiKey_S)
        KEY_CODE_TRANS(KEY_CODE_T, ImGuiKey_T)
        KEY_CODE_TRANS(KEY_CODE_U, ImGuiKey_U)
        KEY_CODE_TRANS(KEY_CODE_V, ImGuiKey_V)
        KEY_CODE_TRANS(KEY_CODE_W, ImGuiKey_W)
        KEY_CODE_TRANS(KEY_CODE_X, ImGuiKey_X)
        KEY_CODE_TRANS(KEY_CODE_Y, ImGuiKey_Y)
        KEY_CODE_TRANS(KEY_CODE_Z, ImGuiKey_Z)
        KEY_CODE_TRANS(KEY_CODE_F1, ImGuiKey_F1)
        KEY_CODE_TRANS(KEY_CODE_F2, ImGuiKey_F2)
        KEY_CODE_TRANS(KEY_CODE_F3, ImGuiKey_F3)
        KEY_CODE_TRANS(KEY_CODE_F4, ImGuiKey_F4)
        KEY_CODE_TRANS(KEY_CODE_F5, ImGuiKey_F5)
        KEY_CODE_TRANS(KEY_CODE_F6, ImGuiKey_F6)
        KEY_CODE_TRANS(KEY_CODE_F7, ImGuiKey_F7)
        KEY_CODE_TRANS(KEY_CODE_F8, ImGuiKey_F8)
        KEY_CODE_TRANS(KEY_CODE_F9, ImGuiKey_F9)
        KEY_CODE_TRANS(KEY_CODE_F10, ImGuiKey_F10)
        KEY_CODE_TRANS(KEY_CODE_F11, ImGuiKey_F11)
        KEY_CODE_TRANS(KEY_CODE_F12, ImGuiKey_F12)
    }
    return ImGuiKey_None;
}