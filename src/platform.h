#ifndef FLOPPY_DEEP_PLATFORM_H
#define FLOPPY_DEEP_PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

//平台类型定义 Platform type definitions
#ifdef _WIN32
    #define PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
#else
    #error "Unsupported platform"
#endif

//窗口结构体（向前声明）Window struct (forward declaration)
struct PlatformWindow;

//窗口创建参数 Window creation parameters
struct WindowDesc {
    int width;
    int height;
    const char* title;
};

//平台初始化/关闭 Platform initialize/shutdown
bool platform_init(void);
void platform_shutdown(void);

//窗口函数 Window functions
struct PlatformWindow* window_create(const struct WindowDesc* desc);
void window_destroy(struct PlatformWindow* window);
bool window_process_events(struct PlatformWindow* window);

//获取原生窗口句柄（仅Windows）Get native window handle (Windows only)
#ifdef PLATFORM_WINDOWS
    //HWND 将在 platform.c 中通过包含 windows.h 定义
    //HWND will be defined in platform.c by including windows.h
    void* window_get_hwnd(struct PlatformWindow* window);
#endif

//系统渲染上下文函数 System rendering context functions
void* render_context_get_native(struct PlatformWindow* window);

#endif //FLOPPY_DEEP_PLATFORM_H
