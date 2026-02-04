#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <windows.h>
#include <plutovg.h>

typedef struct {
    plutovg_surface_t* surface;
    plutovg_canvas_t* canvas;
    int width;
    int height;
} GraphicsContext;
// ¿ò¼Üº¯Êý
GraphicsContext* Graphics_Create(HWND hwnd);
void Graphics_Resize(GraphicsContext* ctx, int width, int height);
void Graphics_BeginDraw(GraphicsContext* ctx);
void* Graphics_GetCanvas(GraphicsContext* ctx);
void Graphics_EndDraw(GraphicsContext* ctx);
void Graphics_Render(HWND hwnd, GraphicsContext* ctx);
void Graphics_Destroy(GraphicsContext* ctx);
typedef void (*DrawFunction)(plutovg_canvas_t* canvas);
void Graphics_Execute(HWND hwnd, GraphicsContext* ctx, DrawFunction draw_func);

#endif
