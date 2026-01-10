#ifndef FLOPPY_DEEP_EDITOR_H
#define FLOPPY_DEEP_EDITOR_H

#include <stdbool.h>

bool Editor_Init(void);
void Editor_Shutdown(void);
void Editor_RunFrame(void);
bool Editor_IsRunning(void);

#endif
