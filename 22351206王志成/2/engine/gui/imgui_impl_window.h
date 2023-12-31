#pragma once

#include <imgui.h>

#include <window.h>
#include <input.h>

#define HAS_SDL_IMPL

class ImGuiRenderer;

struct ImGuiWindowData
{
    size_t Time{0U};
    int MouseButtonsDown{0};
    int PendingMouseLeaveFrame{0};
    char *ClipboardTextData{0};
};

class ImGuiWindow
{
public:
    ImGuiWindow(Window &window, ImGuiRenderer &renderer);
    ~ImGuiWindow();

    void newFrame();

private:
    void updateMouseData();
    void updateMouseCursor();
    void updateGamePads();

    eMouseCursor mouseCursorTranslator(ImGuiMouseCursor cursor);
    ImGuiKey keyCodeTranslatorBack(eKeyCode keyCode);

    Window *m_windowHandle;
    InputSystem *m_inputHandle;
    std::unique_ptr<ImGuiRenderer> m_rendererHandle;

    enum WindowBackendType
    {
        WINDOW_BACKEND_TYPE_UNKNOWN,
        WINDOW_BACKEND_TYPE_SDL
    } m_windowBackendType{WINDOW_BACKEND_TYPE_UNKNOWN};
};