#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>

#include "window.h"

class SDLWindow : public Window
{
public:
    SDLWindow(const char *title, const std::pair<uint32_t, uint32_t> &pos, const std::pair<uint32_t, uint32_t> &size = {1024U, 768U}, const uint8_t &stateFlag = WINDOW_FLAG_CENTERED | WINDOW_FLAG_RESIZEABLE);
    ~SDLWindow()
    {
        if (m_windowHandle)
        {
            close();
        }
    }

    vk::SurfaceKHR createSurface(std::shared_ptr<vk::Instance> instance) override final;
    std::vector<const char *> getRequiredInstanceExtensions()
    {
        std::vector<const char *> res{};
        uint32_t count;
        SDL_Vulkan_GetInstanceExtensions(m_windowHandle, &count, nullptr);
        res.resize(count);
        SDL_Vulkan_GetInstanceExtensions(m_windowHandle, &count, res.data());
        return res;
    }

    void processEvents() override final;

    void show() override final { SDL_ShowWindow(m_windowHandle); }
    void close() override final;

    void setTitle(const char *title) override final { SDL_SetWindowTitle(m_windowHandle, title); }
    void moveTo(const std::pair<uint32_t, uint32_t> &pos) override final { SDL_SetWindowPosition(m_windowHandle, static_cast<int32_t>(pos.first), static_cast<int32_t>(pos.second)); }
    void resize(const std::pair<uint32_t, uint32_t> &size) override final { SDL_SetWindowSize(m_windowHandle, static_cast<int32_t>(size.first), static_cast<int32_t>(size.second)); }

    void *getWindowHandle() const override final { return m_windowHandle; }

    std::pair<uint32_t, uint32_t> getSize() const override final
    {
        int32_t width, height;
        SDL_GetWindowSize(m_windowHandle, &width, &height);
        return {width, height};
    }
    std::pair<uint32_t, uint32_t> getDisplaySize() const override final
    {
        int32_t width, height;
        SDL_Vulkan_GetDrawableSize(m_windowHandle, &width, &height);
        return {width, height};
    }
    std::pair<uint32_t, uint32_t> getPosition() const override final
    {
        int32_t x, y;
        SDL_GetWindowPosition(m_windowHandle, &x, &y);
        return {x, y};
    }
    uint32_t getWindowState() const override final { return SDL_GetWindowFlags(m_windowHandle); }
    bool isFocused() const override final { return (getWindowState() & SDL_WINDOW_INPUT_FOCUS) || (getWindowState() & SDL_WINDOW_MOUSE_FOCUS) || (SDL_GetKeyboardFocus() == m_windowHandle); }
    bool isMinimized() const override final { return (getWindowState() & SDL_WINDOW_MINIMIZED); }
    MonitorProperty getMonitorProperty(uint32_t monitor_index) const override final;
    size_t getCurrentTime() const override final { return SDL_GetPerformanceCounter(); }
    size_t getFrequency() const { return SDL_GetPerformanceFrequency(); }

    float getDPIFactor() const override final;
    std::pair<float, float> getContentScaleFactor() const override final;

private:
    SDL_Window *m_windowHandle{nullptr};
};