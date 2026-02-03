#define PLUTOVG_BUILD_STATIC
#include <plutovg.h>
#include <windows.h>
#include "../res/graphics.h"

GraphicsContext* Graphics_Create(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    GraphicsContext* ctx = malloc(sizeof(GraphicsContext));

    ctx->width = rect.right - rect.left;
    ctx->height = rect.bottom - rect.top;

    ctx->surface = plutovg_surface_create(ctx->width, ctx->height);
    ctx->canvas = plutovg_canvas_create(ctx->surface);

    HDC hdc = GetDC(hwnd);
    ctx->hMemDC = CreateCompatibleDC(hdc);
    ctx->hBitmap = CreateCompatibleBitmap(hdc, ctx->width, ctx->height);
    SelectObject(ctx->hMemDC, ctx->hBitmap);
    ReleaseDC(hwnd, hdc);

    return ctx;
}

void Graphics_Render(HWND hwnd, GraphicsContext* ctx)
{
    HDC hdc = GetDC(hwnd);

    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = ctx->width;
    bmi.bmiHeader.biHeight = -ctx->height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SetDIBits(hdc, ctx->hBitmap, 0, ctx->height,
              plutovg_surface_get_data(ctx->surface), &bmi, DIB_RGB_COLORS);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    BitBlt(hdc, clientRect.left, clientRect.top,
           clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
           ctx->hMemDC, 0, 0, SRCCOPY);

    ReleaseDC(hwnd, hdc);
}

void Graphics_Destroy(GraphicsContext* ctx)
{
    plutovg_canvas_destroy(ctx->canvas);
    plutovg_surface_destroy(ctx->surface);
    DeleteObject(ctx->hBitmap);
    DeleteDC(ctx->hMemDC);
    free(ctx);
}

void Graphics_Execute(HWND hwnd, GraphicsContext* ctx, void (*draw_func)(plutovg_canvas_t*))
{
    draw_func(ctx->canvas);
    Graphics_Render(hwnd, ctx);
}
