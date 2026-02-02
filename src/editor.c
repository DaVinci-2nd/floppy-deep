//可视化编辑器
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateChildWindows(HWND hwnd);
HWND objectsWindow;
HWND sceneWindow;
HWND catalogWindow;
HWND fileWindow;

void create_editor_window()
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "EditorWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);

    //主窗口
    HWND hwnd = CreateWindow
    (
        "EditorWindowClass",
        "Floppy Deep Editor",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        NULL, NULL, wc.hInstance, NULL
    );

    // 菜单栏
    HMENU hMenuBar = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hEditMenu = CreatePopupMenu();
    HMENU hAboutMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, 1001, "Open");
    AppendMenu(hFileMenu, MF_STRING, 1002, "Save");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, 1003, "Quite");

    AppendMenu(hEditMenu, MF_STRING, 2001, "Revoke");
    AppendMenu(hEditMenu, MF_STRING, 2002, "Redo");

    AppendMenu(hAboutMenu, MF_STRING, 3001, "Info");
    AppendMenu(hAboutMenu, MF_STRING, 3002, "Help");

    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, "File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEditMenu, "Edit");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hAboutMenu, "About");

    // 设置菜单栏
    SetMenu(hwnd, hMenuBar);

    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    int clientWidth = rcClient.right - rcClient.left;
    int clientHeight = rcClient.bottom - rcClient.top;

    //子窗口
    objectsWindow = CreateWindow(
        "STATIC",
        "Objects",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0, 100, 100,
        hwnd, NULL, wc.hInstance, NULL
    );

    sceneWindow = CreateWindow
    (
        "STATIC",
        "Scene",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0, 100, 100,
        hwnd, NULL, wc.hInstance, NULL
    );

    catalogWindow = CreateWindow
    (
        "STATIC",
        "Catalog",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0, 100, 100,
        hwnd, NULL, wc.hInstance, NULL
    );

    fileWindow = CreateWindow
    (
        "STATIC",
        "File",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0, 100, 100,
        hwnd, NULL, wc.hInstance, NULL
    );

    UpdateChildWindows(hwnd);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

void UpdateChildWindows(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right;
    int h = rc.bottom;

    MoveWindow(objectsWindow, 0, 0, w/3, h/2, TRUE);
    MoveWindow(sceneWindow, w/3, 0, w*2/3, h/2, TRUE);
    MoveWindow(catalogWindow, 0, h/2, w/3, h/2, TRUE);
    MoveWindow(fileWindow, w/3, h/2, w*2/3, h/2, TRUE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
 {
    switch(uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            UpdateChildWindows(hwnd);
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
