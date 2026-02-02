#include <windows.h>

void create_editor_window();

int main(int argc, char* argv[])
 {
    create_editor_window();
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
