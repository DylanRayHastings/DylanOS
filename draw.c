#include "common.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

DWORD WINAPI DrawLoop(LPVOID lp) {
    RECT rect; GetClientRect(g_hwnd, &rect);
    int W = rect.right;
    int H = rect.bottom;
    srand((unsigned)time(NULL));

    show_typing_splash(g_hwnd, g_hdc);
    show_bsod(g_hwnd, g_hdc);

    // Draw expanding rotating sphere
    for (int phase = 0; phase < 60; phase++) {
        FillRect(g_hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        for (int u = 0; u < num_points; u++) {
            double theta = u * M_PI / (num_points - 1);
            for (int v = 0; v < num_points; v++) {
                double phi = v * 2 * M_PI / (num_points - 1);
                double x = radius * sin(theta) * cos(phi);
                double y = radius * sin(theta) * sin(phi);
                double z = radius * cos(theta);
                double xr = x * cos(angle * rotation_speed) - z * sin(angle * rotation_speed);
                double yr = y;
                int sx = (int)xr + W/2;
                int sy = (int)yr + H/2;
                if (sx >= 0 && sx < W && sy >= 0 && sy < H)
                    SetPixel(g_hdc, sx, sy, RGB(255,255,255));
            }
        }
        GdiFlush(); process_pending_messages();
        radius += W/200;
        angle += 0.1;
        Sleep(16);
    }

    // Particle explosion outward
    int tot = num_points * num_points;
    typedef struct { double x,y,vx,vy; } Par;
    Par* p = malloc(tot * sizeof(Par));
    int idx = 0;
    for (int u = 0; u < num_points; u++) {
        double th = u * M_PI / (num_points - 1);
        for (int v = 0; v < num_points; v++) {
            double ph = v * 2 * M_PI / (num_points - 1);
            double x0 = radius * sin(th) * cos(ph);
            double y0 = radius * sin(th) * sin(ph);
            double z  = radius * cos(th);
            double xr = x0 * cos(angle * rotation_speed) - z * sin(angle * rotation_speed);
            double yr = y0;
            double len = sqrt(xr*xr + yr*yr) + 1e-3;
            double s = (rand() % 5) + 2;
            p[idx].x  = xr + W/2;
            p[idx].y  = yr + H/2;
            p[idx].vx = (xr/len) * s;
            p[idx].vy = (yr/len) * s;
            idx++;
        }
    }
    for (int t = 0; t < 60; t++) {
        FillRect(g_hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        BYTE b = (BYTE)max(0,255 - (t*255/60));
        for (int j = 0; j < tot; j++) {
            p[j].x += p[j].vx;
            p[j].y += p[j].vy;
            int xx = (int)p[j].x;
            int yy = (int)p[j].y;
            if (xx>=0 && xx<W && yy>=0 && yy<H)
                SetPixel(g_hdc, xx, yy, RGB(b,b,b));
        }
        GdiFlush(); process_pending_messages(); Sleep(50);
    }
    free(p);

    // Typewriter text centered and sized
    const char* txt[] = {"Built by logistics.","Shaped by code.","Driven by systems."};
    HFONT f2 = CreateFontA((int)(H*0.04),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE,"Consolas");
    HFONT oldFont = (HFONT)SelectObject(g_hdc,f2);
    SetBkMode(g_hdc,TRANSPARENT); SetTextColor(g_hdc,RGB(255,255,255));
    int cw=W/50, y0=H/2-(int)(H*0.06);
    for(int l=0;l<3;l++){
        SIZE sz; GetTextExtentPoint32A(g_hdc,txt[l],strlen(txt[l]),&sz);
        int x0=(W-sz.cx)/2;
        for(int c=0;c<strlen(txt[l]);c++){
            char ch=txt[l][c];
            TextOutA(g_hdc,x0+c*cw,y0+l*(int)(H*0.06),&ch,1);
            GdiFlush(); process_pending_messages(); Sleep(100);
        }
    }
    DeleteObject(f2);

    FillRect(g_hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
    show_login_screen(g_hwnd, g_hdc);
    return 0;
}
