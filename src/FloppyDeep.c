#include "FloppyDeep.h"
#include "platform.h"
#include <stdlib.h>
#include <string.h>

// 引擎内部状态 Engine internal state
static struct {
    enum EngineState state;
    struct PlatformWindow* window;
    struct FloppyDeepConfig config;
    bool should_quit;
} g_engine;

// 引擎初始化 Engine initialization
bool FloppyDeep_Init(const struct FloppyDeepConfig* config) {
    if (g_engine.state != ENGINE_STATE_UNINITIALIZED) {
        return false;
    }

    // 复制配置 Copy configuration
    g_engine.config = *config;

    // 初始化平台层 Initialize platform layer
    if (!platform_init()) {
        return false;
    }

    // 创建窗口 Create window
    struct WindowDesc window_desc = {
        .width = config->window_width,
        .height = config->window_height,
        .title = config->window_title
    };

    g_engine.window = window_create(&window_desc);
    if (!g_engine.window) {
        platform_shutdown();
        return false;
    }

    // 初始化渲染系统 Initialize render system
    if (!RenderSystem_Init()) {
        window_destroy(g_engine.window);
        platform_shutdown();
        return false;
    }

    g_engine.state = ENGINE_STATE_INITIALIZED;
    g_engine.should_quit = false;

    return true;
}

// 引擎关闭 Engine shutdown
void FloppyDeep_Shutdown(void) {
    if (g_engine.state == ENGINE_STATE_UNINITIALIZED) {
        return;
    }

    RenderSystem_Shutdown();

    if (g_engine.window) {
        window_destroy(g_engine.window);
        g_engine.window = NULL;
    }

    platform_shutdown();

    g_engine.state = ENGINE_STATE_SHUTDOWN;
}

// 引擎主循环 Engine main loop
void FloppyDeep_Run(void) {
    if (g_engine.state != ENGINE_STATE_INITIALIZED &&
        g_engine.state != ENGINE_STATE_RUNNING) {
        return;
    }

    g_engine.state = ENGINE_STATE_RUNNING;

    // 主循环 Main loop
    while (!g_engine.should_quit) {
        // 处理窗口事件 Process window events
        if (!window_process_events(g_engine.window)) {
            g_engine.should_quit = true;
            break;
        }

        // 渲染帧 Render frame
        // 注意：此处将调用系统原生渲染API进行渲染
        // Note: Here we will call the system native rendering API for rendering
    }

    g_engine.state = ENGINE_STATE_INITIALIZED;
}

// 引擎状态查询 Engine state query
enum EngineState FloppyDeep_GetState(void) {
    return g_engine.state;
}

// 渲染系统初始化 Render system initialization
bool RenderSystem_Init(void) {
    // 获取系统原生渲染上下文 Get system native rendering context
    void* render_context = render_context_get_native(g_engine.window);
    if (!render_context) {
        return false;
    }

    // 此处将初始化系统原生渲染API
    // Here we will initialize the system native rendering API
    // 例如：Windows上使用GDI，Linux上使用Xlib
    // Example: Use GDI on Windows, Xlib on Linux

    return true;
}

// 渲染系统关闭 Render system shutdown
void RenderSystem_Shutdown(void) {
    // 清理渲染资源 Clean up rendering resources
}
