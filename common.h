#ifndef DYLANOS_COMMON_H
#define DYLANOS_COMMON_H

#include <windows.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIDTH 800
#define HEIGHT 600

// Forward declarations
void process_pending_messages();
void show_typing_splash(HWND hwnd, HDC hdc);
void show_bsod(HWND hwnd, HDC hdc);
void show_login_screen(HWND hwnd, HDC hdc);
DWORD WINAPI DrawLoop(LPVOID lpParam);

// Globals
extern HWND   g_hwnd;
extern HDC    g_hdc;
extern double angle;
extern double radius;
extern double rotation_speed;
extern int    num_points;

#endif // DYLANOS_COMMON_H
