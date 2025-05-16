#include <windows.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIDTH 800
#define HEIGHT 600

// Forward declarations
void process_pending_messages();
void show_typing_splash();
void show_bsod();
void show_login_screen();
DWORD WINAPI DrawLoop(LPVOID lpParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

// Globals
HWND g_hwnd;
HDC  g_hdc;
HANDLE draw_thread;
double angle = 0.0;
double rotation_speed = 0.05;
int radius = 120;
int num_points = 40;

// Process pending messages
void process_pending_messages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// Terminal splash
void show_typing_splash() {
    RECT rect; GetClientRect(g_hwnd, &rect);
    FillRect(g_hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    HFONT font = CreateFontA(20,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE,"Consolas");
    SelectObject(g_hdc, font);
    SetBkMode(g_hdc, OPAQUE);
    SetBkColor(g_hdc, RGB(0,0,0));

    const char* lines[] = {
        "[  OK  ] Started Journal Service.",
        "[  OK  ] Mounted /boot.",
        "[  OK  ] Reached target Local File Systems.",
        "[  OK  ] Checking network interfaces.",
        "[  OK  ] Starting firewall service.",
        "[FAILED] Failed to start Load Kernel Modules.",
        "[  OK  ] Started Create Volatile Files and Directories.",
        "[FAILED] Failed to authenticate TPM.",
        "[  OK  ] Reached target System Initialization.",
        "[FAILED] Kernel panic imminent.",
        "[  CRASH ] System encountered a critical error.",
        ""
    };
    int lh=25, max=rect.bottom/lh;
    for(int i=0;i<sizeof(lines)/sizeof(lines[0]);i++){
        if(i<max) {
            SetTextColor(g_hdc, strstr(lines[i],"[FAILED]")?RGB(255,0,0):RGB(0,255,0));
            TextOutA(g_hdc,20,i*lh,lines[i],strlen(lines[i]));
        } else {
            BitBlt(g_hdc,0,0,rect.right,rect.bottom-lh,g_hdc,0,lh,SRCCOPY);
            RECT clr={0,rect.bottom-lh,rect.right,rect.bottom};
            FillRect(g_hdc,&clr,(HBRUSH)GetStockObject(BLACK_BRUSH));
            SetTextColor(g_hdc, strstr(lines[i],"[FAILED]")?RGB(255,0,0):RGB(0,255,0));
            TextOutA(g_hdc,20,rect.bottom-lh,lines[i],strlen(lines[i]));
        }
        GdiFlush(); process_pending_messages(); Sleep(400);
    }
    DeleteObject(font);
}

// BSOD
void show_bsod() {
    RECT rect; GetClientRect(g_hwnd, &rect);
    FillRect(g_hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SetBkMode(g_hdc, OPAQUE);
    SetBkColor(g_hdc, RGB(0,0,0));
    SetTextColor(g_hdc, RGB(255,255,255));
    const char* bsod[] = {
        "Kernel panic - not syncing: Attempted to kill init!",
        "",
        "Call Trace:",
        " panic+0x101/0x2e3",
        " do_exit+0xa61/0xa70",
        " end Kernel panic - not syncing: Attempted to kill init!"
    };
    for(int i=0;i<sizeof(bsod)/sizeof(bsod[0]);i++){
        TextOutA(g_hdc,20,50+i*22,bsod[i],strlen(bsod[i]));
        GdiFlush(); process_pending_messages(); Sleep(300);
    }
    Sleep(500);
}

// Login screen
void show_login_screen() {
    RECT rect; GetClientRect(g_hwnd,&rect);
    FillRect(g_hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
    HFONT f = CreateFontA(28,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE,"Consolas");
    SelectObject(g_hdc,f);
    SetBkMode(g_hdc,TRANSPARENT);
    SetTextColor(g_hdc,RGB(255,255,255));
    const char* p1="Enter password: "; const char* p2="******* (press SPACE)";
    SIZE s1,s2; GetTextExtentPoint32A(g_hdc,p1,strlen(p1),&s1);
    GetTextExtentPoint32A(g_hdc,p2,strlen(p2),&s2);
    int x=(WIDTH-(s1.cx+s2.cx))/2; int y=(HEIGHT-s1.cy)/2;
    TextOutA(g_hdc,x,y,p1,strlen(p1));
    TextOutA(g_hdc,x+s1.cx,y,p2,strlen(p2));
    GdiFlush();
    while(!(GetAsyncKeyState(VK_SPACE)&0x8000)){process_pending_messages();Sleep(16);}    
    DeleteObject(f);
}

// DrawLoop
DWORD WINAPI DrawLoop(LPVOID){
    RECT rect; GetClientRect(g_hwnd,&rect); srand(time(NULL));
    show_typing_splash(); show_bsod();
    // sphere zoom
    for(int phase=0;phase<60;phase++){
        FillRect(g_hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
        for(int u=0;u<num_points;u++){
            double th=u*M_PI/(num_points-1);
            for(int v=0;v<num_points;v++){
                double ph=v*2*M_PI/(num_points-1);
                double x=radius*sin(th)*cos(ph);
                double y=radius*sin(th)*sin(ph);
                double z=radius*cos(th);
                double xr=x*cos(angle*rotation_speed)-z*sin(angle*rotation_speed);
                double yr=y;
                int sx=(int)xr+WIDTH/2, sy=(int)yr+HEIGHT/2;
                if(sx>=0&&sx<WIDTH&&sy>=0&&sy<HEIGHT)SetPixel(g_hdc,sx,sy,RGB(255,255,255));
            }
        }
        GdiFlush(); process_pending_messages(); radius+=5; angle+=0.1; Sleep(16);
    }
    // explosion
    int tot=num_points*num_points;
    typedef struct{double x,y,vx,vy;}Par; Par*p=malloc(tot*sizeof(Par));int i=0;
    for(int u=0;u<num_points;u++)for(int v=0;v<num_points;v++){double th=u*M_PI/(num_points-1),ph=v*2*M_PI/(num_points-1);
        double x0=radius*sin(th)*cos(ph),y0=radius*sin(th)*sin(ph),z=radius*cos(th);
        double xr=x0*cos(angle*rotation_speed)-z*sin(angle*rotation_speed);
        double yr=y0; double len=sqrt(xr*xr+yr*yr)+1e-3; double s=(rand()%5)+2;
        p[i].x=xr+WIDTH/2; p[i].y=yr+HEIGHT/2; p[i].vx=(xr/len)*s; p[i].vy=(yr/len)*s; i++;}
    for(int t=0;t<60;t++){
        FillRect(g_hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
        BYTE b=(BYTE)max(0,255-(t*255/60));
        for(int j=0;j<tot;j++){p[j].x+=p[j].vx; p[j].y+=p[j].vy;
            int xx=(int)p[j].x, yy=(int)p[j].y;
            if(xx>=0&&xx<WIDTH&&yy>=0&&yy<HEIGHT)SetPixel(g_hdc,xx,yy,RGB(b,b,b));}
        GdiFlush(); process_pending_messages(); Sleep(50);
    }
    free(p);
    // type text
    const char* txt[] = {"Built by logistics.","Shaped by code.","Driven by systems."};
    HFONT f2=CreateFontA(24,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE,"Consolas");
    SelectObject(g_hdc,f2); SetBkMode(g_hdc,TRANSPARENT); SetTextColor(g_hdc,RGB(255,255,255));
    int charWidth = 14;
    SelectObject(g_hdc,f2); SetBkMode(g_hdc,TRANSPARENT); SetTextColor(g_hdc,RGB(255,255,255));
    int y0=HEIGHT/2-30;
    for(int l=0;l<3;l++){
        SIZE s; GetTextExtentPoint32A(g_hdc,txt[l],strlen(txt[l]),&s);
        int x=(WIDTH-s.cx)/2;
        for(int c=0;c<strlen(txt[l]);c++){
            char ch=txt[l][c]; TextOutA(g_hdc,x+c*charWidth,y0+l*30,&ch,1);
            GdiFlush(); process_pending_messages(); Sleep(100);
        }
    }
    DeleteObject(f2);
    // login
    show_login_screen();
    // main
    FillRect(g_hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH)); GdiFlush();
    return 0;
}
// WindowProc
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
    if(msg==WM_DESTROY){PostQuitMessage(0);return 0;}return DefWindowProc(hwnd,msg,wp,lp);
}
// WinMain
int WINAPI WinMain(HINSTANCE hI,HINSTANCE,LPSTR,int nShow){
    WNDCLASS wc={0}; wc.lpfnWndProc=WindowProc; wc.hInstance=hI;
    wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH); wc.lpszClassName="DylanOSWindow";
    RegisterClass(&wc);
    HWND hwnd=CreateWindowEx(0,"DylanOSWindow","DylanOS",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        CW_USEDEFAULT,CW_USEDEFAULT,WIDTH+20,HEIGHT+40,
        NULL,NULL,hI,NULL);
    if(!hwnd) return 0;
    ShowWindow(hwnd,nShow); UpdateWindow(hwnd);
    g_hwnd=hwnd; g_hdc=GetDC(hwnd);
    draw_thread=CreateThread(NULL,0,DrawLoop,NULL,0,NULL);
    MSG msg; while(GetMessage(&msg,NULL,0,0)){TranslateMessage(&msg);DispatchMessage(&msg);} 
    ReleaseDC(hwnd,g_hdc); CloseHandle(draw_thread);
    return 0;
}
