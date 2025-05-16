#include "common.h"
#include <string.h>

void show_bsod(HWND hwnd, HDC hdc) {
    const char* bsod[] = {
        "Kernel panic - not syncing: Attempted to kill init!",
        "",
        "Call Trace:",
        " panic+0x101/0x2e3",
        " do_exit+0xa61/0xa70",
        " end Kernel panic - not syncing: Attempted to kill init!",
        ""
    };
    RECT rect; GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, RGB(0,0,0));
    SetTextColor(hdc, RGB(255,255,255));

    int count = sizeof(bsod)/sizeof(bsod[0]);
    for (int i = 0; i < count; i++) {
        TextOutA(hdc, 20, 50 + i*22, bsod[i], strlen(bsod[i]));
        GdiFlush(); process_pending_messages(); Sleep(300);
    }
    Sleep(500);
}
