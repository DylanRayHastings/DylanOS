// login.c
#include "common.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define DOT_SPACING    40
#define DOT_RADIUS      3
#define PUSH_DISTANCE 100
#define RETURN_SPEED   0.1f
#define PUSH_SMOOTH    0.2f
#define FRAME_DELAY_MS 33  // ~30 FPS

typedef struct { float x0,y0,x,y; } Dot;

void show_login_screen(HWND hwnd, HDC hdc) {
    ShowCursor(TRUE);
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    RECT rect; GetClientRect(hwnd, &rect);
    int W = rect.right, H = rect.bottom;

    // Offscreen buffer
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, W, H);
    SelectObject(memDC, memBmp);

    // Initialize dot grid
    int cols = W / DOT_SPACING + 2;
    int rows = H / DOT_SPACING + 2;
    int count = cols * rows;
    Dot* dots = malloc(count * sizeof(Dot));
    int idx = 0;
    for (int i = 0; i < cols; i++)
        for (int j = 0; j < rows; j++)
            dots[idx++] = (Dot){i*(float)DOT_SPACING, j*(float)DOT_SPACING, i*(float)DOT_SPACING, j*(float)DOT_SPACING};

    HFONT promptFont = CreateFontA(H*0.04f,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Consolas");

    MSG msg; POINT cursor;
    while (1) {
        // Arrow cursor
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        ShowCursor(TRUE);

        // Pump messages
        while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
            if (msg.message==WM_QUIT) goto END;
            TranslateMessage(&msg); DispatchMessage(&msg);
        }
        if (GetAsyncKeyState(VK_SPACE)&0x8000) break;
        GetCursorPos(&cursor); ScreenToClient(hwnd,&cursor);

        // Draw background+dots
        FillRect(memDC,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
        HBRUSH db=CreateSolidBrush(RGB(255,255,255));
        SelectObject(memDC,db);
        for (int k=0;k<count;k++){
            float dx=dots[k].x0-cursor.x, dy=dots[k].y0-cursor.y, dist=sqrtf(dx*dx+dy*dy);
            float tx=dots[k].x0, ty=dots[k].y0;
            if (dist<PUSH_DISTANCE){
                float push=(PUSH_DISTANCE-dist)/PUSH_DISTANCE, ux=dx/(dist+0.01f), uy=dy/(dist+0.01f);
                tx+=ux*push*PUSH_DISTANCE; ty+=uy*push*PUSH_DISTANCE;
            }
            dots[k].x+=(tx-dots[k].x)*PUSH_SMOOTH; dots[k].y+=(ty-dots[k].y)*PUSH_SMOOTH;
            Ellipse(memDC, (int)(dots[k].x-DOT_RADIUS),(int)(dots[k].y-DOT_RADIUS),
                         (int)(dots[k].x+DOT_RADIUS),(int)(dots[k].y+DOT_RADIUS));
        }
        DeleteObject(db);

        // Top bar
        HBRUSH tb=CreateSolidBrush(RGB(255,255,255)); FillRect(memDC,&(RECT){0,0,W,H/20},tb); DeleteObject(tb);
        // Clock
        SYSTEMTIME st; GetLocalTime(&st);
        char tbuf[9]; sprintf(tbuf,"%02d:%02d:%02d",st.wHour,st.wMinute,st.wSecond);
        HFONT tf=CreateFontA(H*0.02f,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Consolas");
        HFONT oldtf=SelectObject(memDC,tf);
        SetTextColor(memDC,RGB(0,0,0)); SetBkMode(memDC,TRANSPARENT);
        SIZE tsz; GetTextExtentPoint32A(memDC,tbuf,8,&tsz);
        TextOutA(memDC,W-tsz.cx-(int)(W*0.05f),(H/20-tsz.cy)/2,tbuf,8);
        SelectObject(memDC,oldtf); DeleteObject(tf);

        // Power
        int bs=W*0.025f, bx=W-bs-W*0.05f, by=(H/20-bs)/2;
        HPEN pn=CreatePen(PS_SOLID,2,RGB(0,0,0)); SelectObject(memDC,pn);
        SelectObject(memDC,(HBRUSH)GetStockObject(NULL_BRUSH));
        Ellipse(memDC,bx,by,bx+bs,by+bs); MoveToEx(memDC,bx+bs/2,by+4,NULL);
        LineTo(memDC,bx+bs/2,by+bs-4); DeleteObject(pn);

        // Prompt
        HFONT oldpf=SelectObject(memDC,promptFont);
        const char *pr="Enter password:"; SetTextColor(memDC,RGB(255,255,255)); SetBkMode(memDC,TRANSPARENT);
        SIZE psz; GetTextExtentPoint32A(memDC,pr,strlen(pr),&psz);
        int px=(W-psz.cx)/2, py=H/2; TextOutA(memDC,px,py,pr,strlen(pr));
        FrameRect(memDC,&(RECT){px,py+psz.cy,px+W*0.25f,py+psz.cy+H*0.05f},(HBRUSH)GetStockObject(WHITE_BRUSH));
        SelectObject(memDC,oldpf);

        // Flip
        BitBlt(hdc,0,0,W,H,memDC,0,0,SRCCOPY);
        Sleep(FRAME_DELAY_MS);
    }
    END:
    free(dots); DeleteObject(promptFont); DeleteObject(memBmp); DeleteDC(memDC);
}