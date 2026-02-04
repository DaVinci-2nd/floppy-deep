//可视化编辑器
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <plutovg.h>
#include "../res/graphics.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MDIChildProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateChildWindows(HWND hwnd);
static HWND objectsWindow = NULL;
static HWND sceneWindow = NULL;
static HWND catalogWindow = NULL;
static HWND fileWindow = NULL;

static HWND g_hwndMain = NULL;
static HWND g_hwndMDIClient = NULL;
static GraphicsContext* g_sceneGraphics = NULL;

static HMENU hMenuBar =NULL;
static HMENU hFileMenu = NULL;
static HMENU hEditMenu = NULL;
static HMENU hViveMenu = NULL;

static HWND g_hwndStatusBar = NULL;

void create_objects_window(MDICREATESTRUCT* mcs, RECT rcClient);
void create_scene_window(MDICREATESTRUCT* mcs, RECT rcClient);
void create_catalog_window(MDICREATESTRUCT* mcs, RECT rcClient);
void create_file_window(MDICREATESTRUCT* mcs, RECT rcClient);
static HMENU hAboutMenu = NULL;
static BOOL showObjects = TRUE;
static BOOL showScene = TRUE;
static BOOL showCatalog = TRUE;
static BOOL showFile = TRUE;

//窗口
void create_editor_window()
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "EditorWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);

    WNDCLASS wcChild = {0};
    wcChild.lpfnWndProc = MDIChildProc;
    wcChild.hInstance = GetModuleHandle(NULL);
    wcChild.lpszClassName = "MDIChildClass";
    wcChild.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wcChild);

    //主窗口
    g_hwndMain = CreateWindow
    (
        "EditorWindowClass",
        "Floppy Deep Editor",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        NULL, NULL, wc.hInstance, NULL
    );

    ShowWindow(g_hwndMain, SW_SHOW);
    UpdateWindow(g_hwndMain);

    //状态栏
    g_hwndStatusBar = CreateWindow
    (
        STATUSCLASSNAME,
        "done",
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0,
        g_hwndMain,
        (HMENU)9000,
        wc.hInstance,
        NULL
    );
    SendMessage(g_hwndStatusBar, SB_SETPARTS, 1, (LPARAM)(int[]){-1});

    CLIENTCREATESTRUCT ccs = {0};
    ccs.hWindowMenu = NULL;
    ccs.idFirstChild = 50000;

    g_hwndMDIClient = CreateWindow
    (
        "MDICLIENT",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        0, 0, 0, 0,
        g_hwndMain,
        (HMENU)1,
        wc.hInstance,
        (LPVOID)&ccs
    );

    //菜单栏
    hMenuBar = CreateMenu();
    hFileMenu = CreatePopupMenu();
    hEditMenu = CreatePopupMenu();
    hViveMenu = CreatePopupMenu();
    hAboutMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, 1001, "Open");
    AppendMenu(hFileMenu, MF_STRING, 1002, "Save");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, 1003, "Quit");

    AppendMenu(hEditMenu, MF_STRING, 2001, "Revoke");
    AppendMenu(hEditMenu, MF_STRING, 2002, "Redo");

    AppendMenu(hViveMenu, MF_STRING | (showObjects ? MF_CHECKED : MF_UNCHECKED), 3001, "Objects Window");
    AppendMenu(hViveMenu, MF_STRING | (showScene ? MF_CHECKED : MF_UNCHECKED), 3002, "Scene Window");
    AppendMenu(hViveMenu, MF_STRING | (showCatalog ? MF_CHECKED : MF_UNCHECKED), 3003, "Catalog Window");
    AppendMenu(hViveMenu, MF_STRING | (showFile ? MF_CHECKED : MF_UNCHECKED), 3004, "File Window");

    AppendMenu(hAboutMenu, MF_STRING, 4001, "Info");
    AppendMenu(hAboutMenu, MF_STRING, 4002, "Help");

    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, "File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEditMenu, "Edit");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hViveMenu, "Vive");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hAboutMenu, "About");

    SetMenu(g_hwndMain, hMenuBar);

    RECT rcClient;
    GetClientRect(g_hwndMDIClient, &rcClient);

    MDICREATESTRUCT mcs = {0};
    mcs.szClass = "MDIChildClass";
    mcs.hOwner = wc.hInstance;
    mcs.style = WS_CHILD | WS_VISIBLE | WS_CAPTION | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_BORDER;

    //物品窗口
    create_objects_window(&mcs, rcClient);
    //场景窗口
    create_scene_window(&mcs, rcClient);
    //目录窗口
    create_catalog_window(&mcs, rcClient);
    //文件窗口
    create_file_window(&mcs, rcClient);

    UpdateChildWindows(g_hwndMDIClient);

    g_sceneGraphics = Graphics_Create(sceneWindow);
    SetTimer(g_hwndMain, 1, 16, NULL);
}

void create_objects_window(MDICREATESTRUCT* mcs, RECT rcClient)
{
    mcs->szTitle = "Objects";
    mcs->x = 0;
    mcs->y = 0;
    mcs->cx = rcClient.right/3;
    mcs->cy = rcClient.bottom/2;
    objectsWindow = (HWND)SendMessage(g_hwndMDIClient, WM_MDICREATE, 0, (LPARAM)mcs);
}

void create_scene_window(MDICREATESTRUCT* mcs, RECT rcClient)
{
    mcs->szTitle = "Scene";
    mcs->x = rcClient.right/3;
    mcs->cx = rcClient.right*2/3;
    sceneWindow = (HWND)SendMessage(g_hwndMDIClient, WM_MDICREATE, 0, (LPARAM)mcs);
}

void create_catalog_window(MDICREATESTRUCT* mcs, RECT rcClient)
{
    mcs->szTitle = "Catalog";
    mcs->x = 0;
    mcs->y = rcClient.bottom/2;
    mcs->cx = rcClient.right/3;
    mcs->cy = rcClient.bottom/2;
    catalogWindow = (HWND)SendMessage(g_hwndMDIClient, WM_MDICREATE, 0, (LPARAM)mcs);
}

void create_file_window(MDICREATESTRUCT* mcs, RECT rcClient)
{
    mcs->szTitle = "File";
    mcs->x = rcClient.right/3;
    fileWindow = (HWND)SendMessage(g_hwndMDIClient, WM_MDICREATE, 0, (LPARAM)mcs);
}

void DrawSceneDebug(plutovg_canvas_t* canvas)
{
    plutovg_canvas_set_rgb(canvas, 0.5, 0.5, 0.5);
    plutovg_canvas_paint(canvas);
    plutovg_canvas_set_rgb(canvas, 1.0, 0.0, 0.0);
    plutovg_canvas_rect(canvas, 50, 50, 100, 100);
    plutovg_canvas_fill(canvas);
    plutovg_canvas_set_rgb(canvas, 0.0, 1.0, 0.0);
    plutovg_canvas_arc(canvas, 200, 150, 50, 0, 6.2831853, 0);
    plutovg_canvas_fill(canvas);
}

static double obj_rel[4] = {0, 0, 1.0/3.0, 0.5};
static double scene_rel[4] = {1.0/3.0, 0, 2.0/3.0, 0.5};
static double cat_rel[4] = {0, 0.5, 1.0/3.0, 0.5};
static double file_rel[4] = {1.0/3.0, 0.5, 2.0/3.0, 0.5};
static BOOL first_layout_done = FALSE;

//更新子窗口
void UpdateChildWindows(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right;
    int h = rc.bottom;

    if (!first_layout_done) {
        first_layout_done = TRUE;
    }

    if(objectsWindow) MoveWindow(objectsWindow,
        (int)(obj_rel[0] * w),
        (int)(obj_rel[1] * h),
        (int)(obj_rel[2] * w),
        (int)(obj_rel[3] * h), TRUE);

    if(sceneWindow) MoveWindow(sceneWindow,
        (int)(scene_rel[0] * w),
        (int)(scene_rel[1] * h),
        (int)(scene_rel[2] * w),
        (int)(scene_rel[3] * h), TRUE);

    if(catalogWindow) MoveWindow(catalogWindow,
        (int)(cat_rel[0] * w),
        (int)(cat_rel[1] * h),
        (int)(cat_rel[2] * w),
        (int)(cat_rel[3] * h), TRUE);

    if(fileWindow) MoveWindow(fileWindow,
        (int)(file_rel[0] * w),
        (int)(file_rel[1] * h),
        (int)(file_rel[2] * w),
        (int)(file_rel[3] * h), TRUE);
}

//更新布局
void UpdateWindowProportions(HWND hwnd)
{
    if (!first_layout_done) return;

    RECT rc;
    GetClientRect(g_hwndMDIClient, &rc);
    int w = rc.right;
    int h = rc.bottom;

    if(objectsWindow) {
        RECT rect;
        GetWindowRect(objectsWindow, &rect);
        POINT pt = {rect.left, rect.top};
        ScreenToClient(g_hwndMDIClient, &pt);

        obj_rel[0] = pt.x/(double)w;
        obj_rel[1] = pt.y/(double)h;

        if (!IsIconic(objectsWindow)) {
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            obj_rel[2] = width/(double)w;
            obj_rel[3] = height/(double)h;
        }
    }

    if(sceneWindow) {
        RECT rect;
        GetWindowRect(sceneWindow, &rect);
        POINT pt = {rect.left, rect.top};
        ScreenToClient(g_hwndMDIClient, &pt);

        scene_rel[0] = pt.x/(double)w;
        scene_rel[1] = pt.y/(double)h;

        if (!IsIconic(sceneWindow)) {
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            scene_rel[2] = width/(double)w;
            scene_rel[3] = height/(double)h;
        }
    }

    if(catalogWindow) {
        RECT rect;
        GetWindowRect(catalogWindow, &rect);
        POINT pt = {rect.left, rect.top};
        ScreenToClient(g_hwndMDIClient, &pt);

        cat_rel[0] = pt.x/(double)w;
        cat_rel[1] = pt.y/(double)h;

        if (!IsIconic(catalogWindow)) {
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            cat_rel[2] = width/(double)w;
            cat_rel[3] = height/(double)h;
        }
    }

    if(fileWindow) {
        RECT rect;
        GetWindowRect(fileWindow, &rect);
        POINT pt = {rect.left, rect.top};
        ScreenToClient(g_hwndMDIClient, &pt);

        file_rel[0] = pt.x/(double)w;
        file_rel[1] = pt.y/(double)h;

        if (!IsIconic(fileWindow)) {
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            file_rel[2] = width/(double)w;
            file_rel[3] = height/(double)h;
        }
    }
}


LRESULT CALLBACK MDIChildProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg) {
        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == SC_RESTORE) {
                LRESULT lResult = DefMDIChildProc(hwnd, uMsg, wParam, lParam);
                if (hwnd == objectsWindow || hwnd == sceneWindow ||
                    hwnd == catalogWindow || hwnd == fileWindow) {
                    UpdateChildWindows(g_hwndMDIClient);
                }
                return lResult;
            }
            break;
        case WM_EXITSIZEMOVE:
            UpdateWindowProportions(hwnd);
            break;
        case WM_CLOSE:
            if (IsZoomed(hwnd)) {
                SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
                MSG msg;
                for (int i = 0; i < 10; i++) {
                    if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
            }
            if (hwnd == objectsWindow) {
                showObjects = FALSE;
                CheckMenuItem(hViveMenu, 3001, MF_BYCOMMAND | MF_UNCHECKED);
            }
            else if (hwnd == sceneWindow) {
                showScene = FALSE;
                CheckMenuItem(hViveMenu, 3002, MF_BYCOMMAND | MF_UNCHECKED);
            }
            else if (hwnd == catalogWindow) {
                showCatalog = FALSE;
                CheckMenuItem(hViveMenu, 3003, MF_BYCOMMAND | MF_UNCHECKED);
            }
            else if (hwnd == fileWindow) {
                showFile = FALSE;
                CheckMenuItem(hViveMenu, 3004, MF_BYCOMMAND | MF_UNCHECKED);
            }
            break;
        case WM_DESTROY:
            if (hwnd == objectsWindow) {
                objectsWindow = NULL;
            }
            else if (hwnd == sceneWindow) {
                sceneWindow = NULL;
                if (g_sceneGraphics) {
                    Graphics_Destroy(g_sceneGraphics);
                    g_sceneGraphics = NULL;
                }
            }
            else if (hwnd == catalogWindow) {
                catalogWindow = NULL;
            }
            else if (hwnd == fileWindow) {
                fileWindow = NULL;
            }
            break;
    }
    return DefMDIChildProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_COMMAND:
        {
            RECT rcClient;
            MDICREATESTRUCT mcs;

            GetClientRect(g_hwndMDIClient, &rcClient);

            memset(&mcs, 0, sizeof(mcs));
            mcs.szClass = "MDIChildClass";
            mcs.hOwner = GetModuleHandle(NULL);
            mcs.style = WS_CHILD | WS_VISIBLE | WS_CAPTION | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_BORDER;

            switch (LOWORD(wParam))
            {
                case 3001:
                    showObjects = !showObjects;
                    CheckMenuItem(hViveMenu, 3001, MF_BYCOMMAND | (showObjects ? MF_CHECKED : MF_UNCHECKED));
                    if (showObjects && !objectsWindow) {
                        create_objects_window(&mcs, rcClient);
                        UpdateChildWindows(g_hwndMDIClient);
                    } else if (!showObjects && objectsWindow) {
                        SendMessage(objectsWindow, WM_CLOSE, 0, 0);
                    }
                    break;

                case 3002:
                    showScene = !showScene;
                    CheckMenuItem(hViveMenu, 3002, MF_BYCOMMAND | (showScene ? MF_CHECKED : MF_UNCHECKED));
                    if (showScene && !sceneWindow) {
                        create_scene_window(&mcs, rcClient);
                        UpdateChildWindows(g_hwndMDIClient);
                        g_sceneGraphics = Graphics_Create(sceneWindow);
                    } else if (!showScene && sceneWindow) {
                        SendMessage(sceneWindow, WM_CLOSE, 0, 0);
                    }
                    break;

                case 3003:
                    showCatalog = !showCatalog;
                    CheckMenuItem(hViveMenu, 3003, MF_BYCOMMAND | (showCatalog ? MF_CHECKED : MF_UNCHECKED));
                    if (showCatalog && !catalogWindow) {
                        create_catalog_window(&mcs, rcClient);
                        UpdateChildWindows(g_hwndMDIClient);
                    } else if (!showCatalog && catalogWindow) {
                        SendMessage(catalogWindow, WM_CLOSE, 0, 0);
                    }
                    break;

                case 3004:
                    showFile = !showFile;
                    CheckMenuItem(hViveMenu, 3004, MF_BYCOMMAND | (showFile ? MF_CHECKED : MF_UNCHECKED));
                    if (showFile && !fileWindow) {
                        create_file_window(&mcs, rcClient);
                        UpdateChildWindows(g_hwndMDIClient);
                    } else if (!showFile && fileWindow) {
                        SendMessage(fileWindow, WM_CLOSE, 0, 0);
                    }
                    break;
            }
            break;
        }

        case WM_SIZE:
            if(g_hwndMDIClient)
            {
                int statusHeight = 0;
                if (g_hwndStatusBar)
                {
                    RECT rcStatus;
                    GetWindowRect(g_hwndStatusBar, &rcStatus);
                    statusHeight = rcStatus.bottom - rcStatus.top;
                }

                MoveWindow(g_hwndMDIClient,
                          0,
                          0,
                          LOWORD(lParam),
                          HIWORD(lParam) - statusHeight,
                          TRUE);

                if (g_hwndStatusBar)
                {
                    MoveWindow(g_hwndStatusBar,
                              0,
                              HIWORD(lParam) - statusHeight,
                              LOWORD(lParam),
                              statusHeight,
                              TRUE);
                }

                BOOL anyChildMaximized = FALSE;
                if ( (objectsWindow && IsZoomed(objectsWindow)) ||
                     (sceneWindow && IsZoomed(sceneWindow)) ||
                     (catalogWindow && IsZoomed(catalogWindow)) ||
                     (fileWindow && IsZoomed(fileWindow)) ) {
                    anyChildMaximized = TRUE;
                }
                if (!anyChildMaximized) {
                    UpdateChildWindows(g_hwndMDIClient);
                }

            }
            return 0;
        case WM_TIMER:
            RECT rc;
            GetClientRect(sceneWindow, &rc);
            if(sceneWindow && g_sceneGraphics && IsWindow(sceneWindow) && rc.right > 0 && rc.bottom > 0)
            {
                Graphics_Execute(sceneWindow, g_sceneGraphics, DrawSceneDebug);
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefFrameProc(hwnd, g_hwndMDIClient, uMsg, wParam, lParam);
    }
    return DefFrameProc(hwnd, g_hwndMDIClient, uMsg, wParam, lParam);
}
