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
    HANDLE hThread = CreateThread(NULL, 0, DrawLoop, NULL, 0, NULL);
    if (!hThread) {
        MessageBox(NULL, "Failed to start draw thread.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Main message loop keeps the UI responsive
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Once window is closed, wait for the draw thread to finish
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    // Cleanup and exit
    ReleaseDC(g_hwnd, g_hdc);
    DestroyWindow(g_hwnd);
    return 0;
}

// ANSI fallback entrypoint
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR pCmdLine, int nCmdShow) {
    return WinMain(hInst, hPrev, NULL, nCmdShow);
}
