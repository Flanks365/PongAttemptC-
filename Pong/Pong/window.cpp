#include <windows.h>


bool run = true;

struct Render_State{
    int height, width;
    void* memory;

    BITMAPINFO bitmap_info;
};

Render_State render_state;

#include "renderer.cpp"


//on interaction with indow, this gets called
LRESULT CALLBACK WindowCallBack(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch (uMsg) {
    case WM_CLOSE: {}
    case WM_DESTROY: {
        run = false;
    }break;
    case WM_SIZE: {
        RECT rect;
        GetClientRect(hwnd, &rect);
        render_state.width = rect.right - rect.left;
        render_state.height = rect.bottom - rect.top;

        int size = render_state.width * render_state.height * sizeof(unsigned int);

        if (render_state.memory) {
            VirtualFree(render_state.memory, 0, MEM_RELEASE);
        }

        render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
        render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
        render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
        render_state.bitmap_info.bmiHeader.biPlanes = 1;
        render_state.bitmap_info.bmiHeader.biBitCount = 32;
        render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

        render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    }
    default: {
        result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }

    return result;

};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int   nShowCmd) {

    //created window
    WNDCLASS mainWin = {};
    mainWin.style = CS_HREDRAW | CS_VREDRAW;
    mainWin.lpszClassName = L"Game Window Class";
    mainWin.lpfnWndProc = WindowCallBack;

    // registering window
    RegisterClass(&mainWin);

    //Create window
    HWND window = CreateWindow(mainWin.lpszClassName, L"Pong", WS_VISIBLE | WS_OVERLAPPED | WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, 0, 0, hInstance, 0);
    HDC hdc = GetDC(window);

    //game loop
    while (run) {
        //Input
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        //Simpulate (changes background color)
        render_background();
        clear_screen(0xff00ff);
        draw_rect(50,50,200,500,0xff000f);


        //Render
        StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

    }

}