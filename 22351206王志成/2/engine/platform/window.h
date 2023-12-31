#pragma once

#include <simple_vulkan_packing.h>
#include "input.h"

enum sWindowFlag
{
    WINDOW_FLAG_CENTERED = 0x01,
    WINDOW_FLAG_RESIZEABLE = 0x02,
    WINDOW_FLAG_BORDERLESS = 0x04,
    WINDOW_FLAG_HIDDEN = 0x08,
    WINDOW_FLAG_TOPMOST = 0x10,
    WINDOW_FLAG_ENUM_MAX_BIT = 0x7FFFFFFF
};

struct MonitorProperty
{
    uint32_t x{0U};
    uint32_t y{0U};
    uint32_t width{0U};
    uint32_t height{0U};
    float diagonal_dpi{.0f};
    float horizontal_dpi{.0f};
    float vertical_dpi{0U};
};

class Window
{
public:
    Window(const char *title, const std::pair<uint32_t, uint32_t> &pos, const std::pair<uint32_t, uint32_t> &size, const uint8_t &stateFlag) {}
    virtual ~Window() = default;

    virtual vk::SurfaceKHR createSurface(std::shared_ptr<vk::Instance> instance) = 0;

    virtual void processEvents() = 0;

    virtual void show() = 0;
    virtual void close() = 0;

    virtual void setTitle(const char *title) = 0;
    virtual void moveTo(const std::pair<uint32_t, uint32_t> &pos) = 0;
    virtual void resize(const std::pair<uint32_t, uint32_t> &size) = 0;

    virtual void* getWindowHandle() const = 0;
    virtual std::pair<uint32_t, uint32_t> getSize() const = 0;
    virtual std::pair<uint32_t, uint32_t> getDisplaySize() const = 0;
    virtual std::pair<uint32_t, uint32_t> getPosition() const = 0;
    virtual uint32_t getWindowState() const = 0;
    virtual bool isFocused() const = 0;
    virtual bool isMinimized() const = 0;
    virtual MonitorProperty getMonitorProperty(uint32_t monitor_index) const = 0;
    virtual size_t getCurrentTime() const = 0; 

    virtual float getDPIFactor() const = 0;
    virtual std::pair<float, float> getContentScaleFactor() const { return {1.f, 1.f}; }

protected:
    std::vector<uint32_t> m_monitorIndices{};
};