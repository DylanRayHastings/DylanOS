#include "common.h"
#include <windows.h>
#include <math.h>

// Define globals declared in common.h
HWND   g_hwnd         = NULL;
HDC    g_hdc          = NULL;
double angle          = 0.0;
double radius         = 0.0;
double rotation_speed = 0.05;
int    num_points     = 20;

// Pull Windows messages so the UI thread stays responsive
void process_pending_messages(void) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
