#ifndef FLOPPY_DEEP_RENDER_H
#define FLOPPY_DEEP_RENDER_H

#include <stdbool.h>

// 渲染系统初始化/关闭 Render system init/shutdown
bool Render_Init(void* native_context);
void Render_Shutdown(void);

// 基础绘制函数 Basic drawing functions
void Render_ClearScreen(unsigned char r, unsigned char g, unsigned char b);
void Render_DrawRectangle(int x, int y, int width, int height, unsigned char r, unsigned char g, unsigned char b);

#endif // FLOPPY_DEEP_RENDER_H
