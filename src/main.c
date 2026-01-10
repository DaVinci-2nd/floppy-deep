#include "FloppyDeep.h"
#include <stdio.h>

// 主程序入口 Main program entry
int main(int argc, char* argv[]) {
    // 引擎配置 Engine configuration
    struct FloppyDeepConfig config = {
        .window_width = 800,
        .window_height = 600,
        .window_title = "Floppy Deep Engine",
        .enable_logging = true
    };

    // 初始化引擎 Initialize engine
    if (!FloppyDeep_Init(&config)) {
        fprintf(stderr, "Failed to initialize Floppy Deep Engine.\n");
        return 1;
    }

    // 运行引擎主循环 Run engine main loop
    FloppyDeep_Run();

    // 关闭引擎 Shutdown engine
    FloppyDeep_Shutdown();

    return 0;
}
