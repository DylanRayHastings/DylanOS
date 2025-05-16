#include "common.h"
#include <windows.h>

// Forward declaration for draw loop
DWORD WINAPI DrawLoop(LPVOID lpParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR pCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc   = DefWindowProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "MainClass";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    // Create main window
    g_hwnd = CreateWindow(
        "MainClass", "DylanOS",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
        NULL, NULL, hInst, NULL
    );
    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);

    // Prepare drawing context
    g_hdc = GetDC(g_hwnd);

    // Launch the draw loop on a worker thread
    CreateThread(NULL, 0, DrawLoop, NULL, 0, NULL);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
