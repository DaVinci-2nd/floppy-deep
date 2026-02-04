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

    return ctx;
}

void Graphics_Render(HWND hwnd, GraphicsContext* ctx)
{
    if (!ctx || ctx->width <= 0 || ctx->height <= 0) return;

    HDC hdc = GetDC(hwnd);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = ctx->width;
    bmi.bmiHeader.biHeight = -ctx->height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(hdc,
                      0, 0,
                      ctx->width, ctx->height,
                      0, 0,
                      0, ctx->height,
                      plutovg_surface_get_data(ctx->surface),
                      &bmi,
                      DIB_RGB_COLORS);

    ReleaseDC(hwnd, hdc);
}

void Graphics_Destroy(GraphicsContext* ctx)
{
    plutovg_canvas_destroy(ctx->canvas);
    plutovg_surface_destroy(ctx->surface);
    free(ctx);
}

void Graphics_Execute(HWND hwnd, GraphicsContext* ctx, void (*draw_func)(plutovg_canvas_t*))
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    int new_width = rect.right - rect.left;
    int new_height = rect.bottom - rect.top;

    if (new_width != ctx->width || new_height != ctx->height)
    {
        plutovg_canvas_destroy(ctx->canvas);
        plutovg_surface_destroy(ctx->surface);

        ctx->width = new_width;
        ctx->height = new_height;
        ctx->surface = plutovg_surface_create(new_width, new_height);
        ctx->canvas = plutovg_canvas_create(ctx->surface);
    }

    unsigned char* data = plutovg_surface_get_data(ctx->surface);
    if (data)
    {
        memset(data, 0, ctx->width * ctx->height * 4);
    }

    if (draw_func)
    {
        draw_func(ctx->canvas);
    }

    Graphics_Render(hwnd, ctx);
}

void Graphics_DrawImage(GraphicsContext* ctx, const char* image_data, int image_width, int image_height, int x, int y)
{
    plutovg_surface_t* image = plutovg_surface_create_for_data(
        (unsigned char*)image_data, image_width, image_height, image_width * 4);

    plutovg_paint_t* paint = plutovg_paint_create_texture(image, PLUTOVG_TEXTURE_TYPE_PLAIN, 1.0f, NULL);
    plutovg_canvas_set_paint(ctx->canvas, paint);
    plutovg_canvas_rect(ctx->canvas, x, y, image_width, image_height);
    plutovg_canvas_fill(ctx->canvas);

    plutovg_paint_destroy(paint);
    plutovg_surface_destroy(image);
}
