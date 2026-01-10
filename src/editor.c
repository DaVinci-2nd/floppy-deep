#include "editor.h"
#include "platform.h"
#include <stddef.h>

//在Windows平台包含必要的头文件 Include necessary headers for Windows platform
#ifdef PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

//编辑器内部状态 Editor Internal State
static struct {
    bool is_initialized;
    bool is_running;
    struct PlatformWindow* window;
#ifdef PLATFORM_WINDOWS
    HMENU hMenuBar; //Windows菜单栏句柄 Windows menu bar handle
#endif
} g_editor;

bool Editor_Init(void) {
    //初始化平台层 Initialize platform layer
    platform_init();
    //设置窗口参数 Set window parameters
    struct WindowDesc desc;
    desc.width = 800;
    desc.height = 600;
    desc.title = "Floppy Deep Editor";
    //创建窗口 Create window
    g_editor.window = window_create(&desc);
    //创建菜单栏 Create menu bar
#ifdef PLATFORM_WINDOWS
    {
        //Windows菜单创建 Windows menu creation
        HMENU hMenuBar = CreateMenu();
        HMENU hFileMenu = CreatePopupMenu();
        HMENU hEditMenu = CreatePopupMenu();
        HMENU hAboutMenu = CreatePopupMenu();

        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, "File");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEditMenu, "Edit");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hAboutMenu, "About");

        //使用新函数获取窗口句柄 Use new function to get window handle
        HWND hwnd = (HWND)window_get_hwnd(g_editor.window);
        SetMenu(hwnd, hMenuBar);
        g_editor.hMenuBar = hMenuBar;
    }
#endif
    //设置编辑器状态 Set editor state
    g_editor.is_initialized = true;
    g_editor.is_running = true;
    //返回初始化成功 Return initialization success
    return true;
}

void Editor_Shutdown(void) {
    if (!g_editor.is_initialized) {
        return;
    }
    //销毁菜单栏 Destroy menu bar
#ifdef PLATFORM_WINDOWS
    if (g_editor.hMenuBar) {
        DestroyMenu(g_editor.hMenuBar);
        g_editor.hMenuBar = NULL;
    }
#endif
    //销毁窗口 Destroy window
    if (g_editor.window) {
        window_destroy(g_editor.window);
        g_editor.window = NULL;
    }
    //重置编辑器状态 Reset editor state
    g_editor.is_initialized = false;
    g_editor.is_running = false;
}

void Editor_RunFrame(void) {
    if (!g_editor.is_initialized || !g_editor.is_running) {
        return;
    }
    //处理窗口事件 Process window events
    if (!window_process_events(g_editor.window)) {
        g_editor.is_running = false;
    }
}

bool Editor_IsRunning(void) {
    return g_editor.is_running;
}
