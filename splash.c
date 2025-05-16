#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

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

    HFONT font = CreateFontA(
        20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, "Consolas"
    );
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, RGB(0, 0, 0));

    int lh = 25;
    int total = sizeof(lines) / sizeof(lines[0]);
    srand((unsigned)time(NULL));

    // Render initial lines
    for (int i = 0; i < total; i++) {
        int y = i * lh;
        if (y > rect.bottom - lh) {
            BitBlt(hdc, 0, 0, rect.right, rect.bottom - lh, hdc, 0, lh, SRCCOPY);
            RECT clr = {0, rect.bottom - lh, rect.right, rect.bottom};
            FillRect(hdc, &clr, (HBRUSH)GetStockObject(BLACK_BRUSH));
            y = rect.bottom - lh;
        }
        // Color lines
        if (strstr(lines[i], "[FAILED]")) SetTextColor(hdc, RGB(255, 0, 0));
        else SetTextColor(hdc, RGB(0, 255, 0));
        TextOutA(hdc, 20, y, lines[i], strlen(lines[i]));
        GdiFlush(); process_pending_messages(); Sleep(300);

        // On crash, break to corrupt
        if (strstr(lines[i], "[  CRASH ]")) {
            // Flash crash line
            for (int f = 0; f < 6; f++) {
                SetTextColor(hdc, (f % 2) ? RGB(255, 0, 0) : RGB(255, 255, 255));
                TextOutA(hdc, 20 + rand()%10 - 5, y + rand()%6 - 3, lines[i], strlen(lines[i]));
                GdiFlush(); Sleep(100);
            }
            // Corruption: shake and glitch lines
            char* buffer = _strdup(lines[i]);
            int len = strlen(buffer);
            for (int g = 0; g < 100; g++) {
                buffer[rand() % len] = (char)(33 + rand() % 94);
                // redraw all up to crash
                for (int k = 0; k <= i; k++) {
                    int yy = k * lh;
                    const char* text = (k == i) ? buffer : lines[k];
                    SetTextColor(hdc, (k==i) ? RGB(rand()%256, rand()%256, rand()%256) : (strstr(lines[k],"[FAILED]")?RGB(255,0,0):RGB(0,255,0)));
                    TextOutA(hdc, 20 + rand()%8 - 4, yy + rand()%8 - 4, text, strlen(text));
                }
                GdiFlush(); process_pending_messages(); Sleep(50);
            }
            free(buffer);
            break;
        }
    }

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}
