#include "platform.h"
#include <stdlib.h>
#include <string.h>

//平台特定头文件 Platform-specific headers
#ifdef PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif defined(PLATFORM_LINUX)
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/Xos.h>
    #include <X11/keysym.h>
#endif

//平台窗口结构体 Platform window structure
struct PlatformWindow {
    int width;
    int height;
    bool should_close;
#ifdef PLATFORM_WINDOWS
    HWND hwnd;
    HDC hdc;
#elif defined(PLATFORM_LINUX)
    Display* display;
    Window xwindow;
    GC gc;
#endif
};

//平台全局状态 Platform global state
static struct {
#ifdef PLATFORM_LINUX
    Display* display;
#endif
    bool initialized;
} g_platform;

//平台初始化 Platform initialization
bool platform_init(void) {
    if (g_platform.initialized) {
        return true;
    }

#ifdef PLATFORM_WINDOWS
    //Windows不需要特殊初始化 Windows doesn't need special initialization
#elif defined(PLATFORM_LINUX)
    g_platform.display = XOpenDisplay(NULL);
    if (!g_platform.display) {
        return false;
    }
#endif

    g_platform.initialized = true;
    return true;
}

//平台关闭 Platform shutdown
void platform_shutdown(void) {
    if (!g_platform.initialized) {
        return;
    }

#ifdef PLATFORM_LINUX
    if (g_platform.display) {
        XCloseDisplay(g_platform.display);
        g_platform.display = NULL;
    }
#endif

    g_platform.initialized = false;
}

//窗口创建 Window creation
struct PlatformWindow* window_create(const struct WindowDesc* desc) {
    if (!g_platform.initialized) {
        return NULL;
    }

    struct PlatformWindow* win = (struct PlatformWindow*)calloc(1, sizeof(struct PlatformWindow));
    if (!win) {
        return NULL;
    }

    win->width = desc->width;
    win->height = desc->height;
    win->should_close = false;

#ifdef PLATFORM_WINDOWS
    //Windows窗口创建 Windows window creation
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "FloppyDeepWindowClass";
    RegisterClass(&wc);

    DWORD style = WS_OVERLAPPEDWINDOW;
    RECT rect = {0, 0, desc->width, desc->height};
    AdjustWindowRect(&rect, style, FALSE);

    win->hwnd = CreateWindow(
        "FloppyDeepWindowClass",
        desc->title,
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, NULL, hInstance, NULL);

    if (!win->hwnd) {
        free(win);
        return NULL;
    }

    win->hdc = GetDC(win->hwnd);
    ShowWindow(win->hwnd, SW_SHOW);
    UpdateWindow(win->hwnd);

#elif defined(PLATFORM_LINUX)
    //X11窗口创建 X11 window creation
    win->display = g_platform.display;
    int screen = DefaultScreen(win->display);
    win->xwindow = XCreateSimpleWindow(
        win->display,
        RootWindow(win->display, screen),
        0, 0, desc->width, desc->height, 0,
        BlackPixel(win->display, screen),
        WhitePixel(win->display, screen));

    XStoreName(win->display, win->xwindow, desc->title);
    XSelectInput(win->display, win->xwindow,
        ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask | StructureNotifyMask);

    win->gc = XCreateGC(win->display, win->xwindow, 0, NULL);
    XMapWindow(win->display, win->xwindow);
    XFlush(win->display);
#endif

    return win;
}

//窗口销毁 Window destruction
void window_destroy(struct PlatformWindow* window) {
    if (!window) return;

#ifdef PLATFORM_WINDOWS
    if (window->hdc) {
        ReleaseDC(window->hwnd, window->hdc);
    }
    if (window->hwnd) {
        DestroyWindow(window->hwnd);
    }
#elif defined(PLATFORM_LINUX)
    if (window->gc) {
        XFreeGC(window->display, window->gc);
    }
    if (window->xwindow) {
        XDestroyWindow(window->display, window->xwindow);
    }
#endif

    free(window);
}

//窗口事件处理 Window event processing
bool window_process_events(struct PlatformWindow* window) {
    if (!window || window->should_close) {
        return false;
    }

#ifdef PLATFORM_WINDOWS
    MSG msg;
    while (PeekMessage(&msg, window->hwnd, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            window->should_close = true;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#elif defined(PLATFORM_LINUX)
    XEvent event;
    while (XPending(window->display)) {
        XNextEvent(window->display, &event);
        switch (event.type) {
            case ClientMessage:
                //处理关闭事件 Handle close event
                window->should_close = true;
                break;
            case DestroyNotify:
                window->should_close = true;
                break;
        }
    }
#endif

    return !window->should_close;
}

//获取原生窗口句柄（仅Windows）Get native window handle (Windows only)
#ifdef PLATFORM_WINDOWS
void* window_get_hwnd(struct PlatformWindow* window) {
    if (!window) return NULL;
    return (void*)window->hwnd;
}
#endif

//获取原生渲染上下文 Get native rendering context
void* render_context_get_native(struct PlatformWindow* window) {
    if (!window) return NULL;

#ifdef PLATFORM_WINDOWS
    return (void*)window->hdc;
#elif defined(PLATFORM_LINUX)
    return (void*)window->gc;
#endif
}
