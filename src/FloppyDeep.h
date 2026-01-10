#ifndef FLOPPY_DEEP_ENGINE_H
#define FLOPPY_DEEP_ENGINE_H

#include <stdbool.h>

// 引擎配置结构体 Engine configuration structure
struct FloppyDeepConfig {
    int window_width;
    int window_height;
    const char* window_title;
    bool enable_logging;
};

// 引擎状态枚举 Engine state enum
enum EngineState {
    ENGINE_STATE_UNINITIALIZED,
    ENGINE_STATE_INITIALIZED,
    ENGINE_STATE_RUNNING,
    ENGINE_STATE_SHUTDOWN
};

// 引擎初始化/关闭 Engine initialize/shutdown
bool FloppyDeep_Init(const struct FloppyDeepConfig* config);
void FloppyDeep_Shutdown(void);

// 引擎主循环 Engine main loop
void FloppyDeep_Run(void);

// 引擎状态查询 Engine state query
enum EngineState FloppyDeep_GetState(void);

// 渲染系统初始化（调用系统原生渲染） Render system initialization (using native rendering)
bool RenderSystem_Init(void);
void RenderSystem_Shutdown(void);

#endif // FLOPPY_DEEP_ENGINE_H
