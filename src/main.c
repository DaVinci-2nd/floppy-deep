#include "FloppyDeep.h"
#include "editor.h"

int main(int argc, char* argv[])
{
    //程序模式开关 Program mode switch
    //1为编辑器模式，0为引擎游戏模式 1 for editor mode, 0 for engine game mode
    const int LAUNCH_EDITOR = 1;

    //初始化编辑器模块 Initialize editor module
    Editor_Init();

    //编辑器主循环 Editor main loop
    while (Editor_IsRunning())
    {
        Editor_RunFrame();
    }

    //关闭编辑器模块 Shutdown editor module
    Editor_Shutdown();

    //程序退出 Program exit
    return 0;
}
