#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <any>
#include <cstdint>

#include "input.h"

class MessageHandler
{
private:
    MessageHandler() {}
    MessageHandler(const MessageHandler &) = delete;
    MessageHandler(MessageHandler &&) = delete;
    MessageHandler &operator=(const MessageHandler &) = delete;
    MessageHandler &operator=(MessageHandler &&) = delete;
    ~MessageHandler() {}

    template <typename... Ts>
    using messageCallback = std::pair<std::function<void(Ts..., std::any)>, std::any>;

    using windowResizeCallback = messageCallback<std::any, uint32_t, uint32_t>;
    using windowCloseCallback = messageCallback<std::any>;
    using windowMoveCallback = messageCallback<std::any>;
    using windowFocusCallback = messageCallback<std::any>;
    using mouseMotionCallback = messageCallback<int32_t, int32_t, bool>;
    using mouseWheelCallback = messageCallback<int32_t, int32_t, bool>;
    using mouseButtonCallback = messageCallback<eMouseKeyCode, int32_t, int32_t, bool>;
    using keyCallback = messageCallback<eKeyCode>;
    using textInputCallback = messageCallback<const char *>;

public:
    static MessageHandler &getInstance()
    {
        static MessageHandler instance;
        return instance;
    }

    void onWindowResize(std::any windowHandle, uint32_t w, uint32_t h)
    {
        for (auto &callback : m_windowResizeCallbacks)
            callback.second.first(windowHandle, w, h, callback.second.second);
    }
    void onWindowClose(std::any windowHandle)
    {
        for (auto &callback : m_windowCloseCallbacks)
            callback.second.first(windowHandle, callback.second.second);
    }
    void onWindowMove(std::any windowHandle)
    {
        for (auto &callback : m_windowMoveCallbacks)
            callback.second.first(windowHandle, callback.second.second);
    }
    void onWindowMouseFocusOn(std::any windowHandle)
    {
        for (auto &callback : m_windowMouseFocusOnCallbacks)
            callback.second.first(windowHandle, callback.second.second);
    }
    void onWindowMouseFocusOff(std::any windowHandle)
    {
        for (auto &callback : m_windowMouseFocusOffCallbacks)
            callback.second.first(windowHandle, callback.second.second);
    }
    void onWindowKeyboardFocusOn(std::any windowHandle)
    {
        for (auto &callback : m_windowKeyboardFocusOnCallbacks)
            callback.second.first(windowHandle, callback.second.second);
    }
    void onWindowKeyboardFocusOff(std::any windowHandle)
    {
        for (auto &callback : m_windowKeyboardFocusOffCallbacks)
            callback.second.first(windowHandle, callback.second.second);
    }
    void onMouseMove(int32_t x, int32_t y, bool touch)
    {
        for (auto &callback : m_mouseMotionCallbacks)
            callback.second.first(x, y, touch, callback.second.second);
    }
    void onMouseWheel(int32_t x, int32_t y, bool touch)
    {
        for (auto &callback : m_mouseWheelCallbacks)
            callback.second.first(x, y, touch, callback.second.second);
    }
    void onMouseButtonDown(eMouseKeyCode mouseKeyCode, int32_t x, int32_t y, bool touch)
    {
        for (auto &callback : m_mouseButtonDownCallbacks)
            callback.second.first(mouseKeyCode, x, y, touch, callback.second.second);
    }
    void onMouseButtonUp(eMouseKeyCode mouseKeyCode, int32_t x, int32_t y, bool touch)
    {
        for (auto &callback : m_mouseButtonUpCallbacks)
            callback.second.first(mouseKeyCode, x, y, touch, callback.second.second);
    }
    void onKeyDown(eKeyCode keyCode)
    {
        for (auto &callback : m_keyDownCallbacks)
            callback.second.first(keyCode, callback.second.second);
    }
    void onKeyUp(eKeyCode keyCode)
    {
        for (auto &callback : m_keyUpCallbacks)
            callback.second.first(keyCode, callback.second.second);
    }
    void onTextInput(const char *text)
    {
        for (auto &callback : m_textInputCallbacks)
            callback.second.first(text, callback.second.second);
    }

    size_t addWindowResizeCallback(std::function<void(std::any, uint32_t, uint32_t, std::any)> fn, std::any data)
    {
        m_windowResizeCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeWindowResizeCallback(size_t index) { m_windowResizeCallbacks.erase(index); }

    size_t addWindowCloseCallback(std::function<void(std::any, std::any)> fn, std::any data)
    {
        m_windowCloseCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeWindowCloseCallback(size_t index) { m_windowCloseCallbacks.erase(index); }

    size_t addWindowMoveCallback(std::function<void(std::any, std::any)> fn, std::any data)
    {
        m_windowMoveCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeWindowMoveCallback(size_t index) { m_windowMoveCallbacks.erase(index); }

    size_t addWindowMouseFocusOnCallback(std::function<void(std::any, std::any)> fn, std::any data)
    {
        m_windowMouseFocusOnCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeWindowMouseFocusOnCallback(size_t index) { m_windowMouseFocusOnCallbacks.erase(index); }

    size_t addWindowMouseFocusOffCallback(std::function<void(std::any, std::any)> fn, std::any data)
    {
        m_windowMouseFocusOffCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeWindowMouseFocusOffCallback(size_t index) { m_windowMouseFocusOffCallbacks.erase(index); }

    size_t addWindowKeyboardFocusOnCallback(std::function<void(std::any, std::any)> fn, std::any data)
    {
        m_windowKeyboardFocusOnCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeWindowKeyboardFocusOnCallback(size_t index) { m_windowKeyboardFocusOnCallbacks.erase(index); }

    size_t addWindowKeyboardFocusOffCallback(std::function<void(std::any, std::any)> fn, std::any data)
    {
        m_windowKeyboardFocusOffCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeWindowKeyboardFocusOffCallback(size_t index) { m_windowKeyboardFocusOffCallbacks.erase(index); }

    size_t addMouseMotionCallback(std::function<void(int32_t, int32_t, bool, std::any)> fn, std::any data)
    {
        m_mouseMotionCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeMouseMotionCallback(size_t index) { m_mouseMotionCallbacks.erase(index); }

    size_t addMouseWheelCallback(std::function<void(int32_t, int32_t, bool, std::any)> fn, std::any data)
    {
        m_mouseWheelCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeMouseWheelCallback(size_t index) { m_mouseWheelCallbacks.erase(index); }

    size_t addMouseButtonDownCallback(std::function<void(eMouseKeyCode, int32_t, int32_t, bool, std::any)> fn, std::any data)
    {
        m_mouseButtonDownCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeMouseButtonDownCallback(size_t index) { m_mouseButtonDownCallbacks.erase(index); }

    size_t addMouseButtonUpCallback(std::function<void(eMouseKeyCode, int32_t, int32_t, bool, std::any)> fn, std::any data)
    {
        m_mouseButtonUpCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeMouseButtonUpCallback(size_t index) { m_mouseButtonUpCallbacks.erase(index); }

    size_t addKeyDownCallback(std::function<void(eKeyCode, std::any)> fn, std::any data)
    {
        m_keyDownCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeKeyDownCallback(size_t index) { m_keyDownCallbacks.erase(index); }

    size_t addKeyUpCallback(std::function<void(eKeyCode, std::any)> fn, std::any data)
    {
        m_keyUpCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeKeyUpCallback(size_t index) { m_keyUpCallbacks.erase(index); }

    size_t addTextInputCallback(std::function<void(const char *, std::any)> fn, std::any data)
    {
        m_textInputCallbacks[m_nextCallbackIndex++] = {fn, data};
        return m_nextCallbackIndex - 1;
    }
    void removeTextInputCallback(size_t index) { m_textInputCallbacks.erase(index); }

private:
    std::unordered_map<size_t, windowResizeCallback> m_windowResizeCallbacks;
    std::unordered_map<size_t, windowCloseCallback> m_windowCloseCallbacks;
    std::unordered_map<size_t, windowMoveCallback> m_windowMoveCallbacks;
    std::unordered_map<size_t, windowFocusCallback> m_windowMouseFocusOnCallbacks;
    std::unordered_map<size_t, windowFocusCallback> m_windowMouseFocusOffCallbacks;
    std::unordered_map<size_t, windowFocusCallback> m_windowKeyboardFocusOnCallbacks;
    std::unordered_map<size_t, windowFocusCallback> m_windowKeyboardFocusOffCallbacks;
    std::unordered_map<size_t, mouseMotionCallback> m_mouseMotionCallbacks;
    std::unordered_map<size_t, mouseWheelCallback> m_mouseWheelCallbacks;
    std::unordered_map<size_t, mouseButtonCallback> m_mouseButtonDownCallbacks;
    std::unordered_map<size_t, mouseButtonCallback> m_mouseButtonUpCallbacks;
    std::unordered_map<size_t, keyCallback> m_keyDownCallbacks;
    std::unordered_map<size_t, keyCallback> m_keyUpCallbacks;
    std::unordered_map<size_t, textInputCallback> m_textInputCallbacks;
    size_t m_nextCallbackIndex{0U};
};