#include "common.h"
#include <string.h>

void show_typing_splash(HWND hwnd, HDC hdc) {
    const char* lines[] = {
        "[  OK  ] Started Journal Service.",
        "[  OK  ] Mounted /boot.",
        "[  OK  ] Reached target Local File Systems.",
        "[  OK  ] Checking network interfaces.",
        "[  OK  ] Starting firewall service.",
        "[FAILED] Failed to start Load Kernel Modules.",
        "[FAILED] Failed to authenticate TPM.",
        "[FAILED] Failed to mount encrypted volume.",
        "[FAILED] Failed to start Network Manager.",
        "[FAILED] Kernel panic imminent.",
        "[  CRASH ] System encountered a critical error.",
        ""
    };
    RECT rect; GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    HFONT font = CreateFontA(20,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE,"Consolas");
    SelectObject(hdc, font);
    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, RGB(0,0,0));

    int lh = 25;
    int total = sizeof(lines)/sizeof(lines[0]);
    int max = rect.bottom / lh;
    for (int i = 0; i < total; i++) {
        int y = (i < max) ? i * lh : rect.bottom - lh;
        if (i >= max) {
            BitBlt(hdc, 0, 0, rect.right, rect.bottom-lh, hdc, 0, lh, SRCCOPY);
            RECT clr = {0, rect.bottom-lh, rect.right, rect.bottom};
            FillRect(hdc, &clr, (HBRUSH)GetStockObject(BLACK_BRUSH));
        }
        SetTextColor(hdc,
            strstr(lines[i], "[FAILED]") ? RGB(255,0,0) : RGB(0,255,0));
        TextOutA(hdc, 20, y, lines[i], strlen(lines[i]));
        GdiFlush(); process_pending_messages(); Sleep(400);
    }
    DeleteObject(font);
}
