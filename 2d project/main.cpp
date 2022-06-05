#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <math.h>
#include <cmath>
#include <fstream>
#include <vector>
#include <list>
#include <stack>
using namespace std;
#define MAXENTRIES 600
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("2d project");
int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_CROSS);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_3DFACE;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Graphics Project"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           1366,                 /* The programs width */
           768,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}
bool save(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel = 24)
{
    uint32_t Width = Area.right - Area.left;
    uint32_t Height = Area.bottom - Area.top;

    BITMAPINFO Info;
    BITMAPFILEHEADER Header;
    memset(&Info, 0, sizeof(Info));
    memset(&Header, 0, sizeof(Header));
    Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Info.bmiHeader.biWidth = Width;
    Info.bmiHeader.biHeight = Height;
    Info.bmiHeader.biPlanes = 1;
    Info.bmiHeader.biBitCount = BitsPerPixel;
    Info.bmiHeader.biCompression = BI_RGB;
    Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
    Header.bfType = 0x4D42;
    Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);


    char* Pixels = NULL;
    HDC MemDC = CreateCompatibleDC(Context);
    HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)&Pixels, 0, 0);
    DeleteObject(SelectObject(MemDC, Section));
    BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
    DeleteDC(MemDC);

    std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
    if (hFile.is_open())
    {
        hFile.write((char*)&Header, sizeof(Header));
        hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
        hFile.write(Pixels, (((BitsPerPixel * Width + 31) & ~31) / 8) * Height);
        hFile.close();
        DeleteObject(Section);
        return true;
    }

    DeleteObject(Section);
    return false;
}
void load(HWND hWnd, HDC &hdc)
{
    string fileName;
    cin>>fileName;
    if (fileName == "")
        return ;
    HBITMAP hBitmap;
    hBitmap = (HBITMAP)::LoadImage(NULL, fileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    HDC hLocalDC;
    hLocalDC = CreateCompatibleDC(hdc);
    BITMAP qBitmap;
    int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP),reinterpret_cast<LPVOID>(&qBitmap));
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hLocalDC, hBitmap);
    BOOL qRetBlit = BitBlt(hdc, 0, 0, qBitmap.bmWidth, qBitmap.bmHeight,hLocalDC, 0, 0, SRCCOPY);
    SelectObject (hLocalDC, hOldBmp);
    DeleteDC(hLocalDC);
    DeleteObject(hBitmap);
}

void swap(int& x1, int& y1, int& x2, int& y2)
{
	int tmp = x1;
	x1 = x2;
	x2 = tmp;
	tmp = y1;
	y1 = y2;
	y2 = tmp;
}
int Round (double x)
{
    return (int)(x+0.5);
}
void LineDDA(HDC hdc,int x1,int y1,int x2,int y2,COLORREF color)
{
    int dx=x2-x1;
    int dy=y2-y1;
    SetPixel(hdc,x1,y1,color);
    if(abs(dy)<=abs(dx))
    {
        if(x1>x2)
        {
            swap(x1,y1,x2,y2);
        }
        double x=x1;
        double y=y1;
        double m=(double)dy/dx;
        SetPixel(hdc,x1,y1,color);
        while(x!=x2)
        {
             x++;
             y+=m;
            SetPixel(hdc,x,round(y),color);
        }

    }else{
    if(y1>y2)
        swap(x1,y1,x2,y2);
    double x=x1;
    double y=y1;
   double minv=(double)dx/dy;
    SetPixel(hdc,x1,y1,color);
    while(y!=y2)
    {
         y++;
        x+=minv;
        SetPixel(hdc,round(x),y,color);
    }

    }
}
void Midpoint(HDC hdc,int x1,int y1,int x2,int y2,COLORREF color)
{
    int dx=abs(x2-x1),dy=abs(y2-y1),p=0;
    if(dx<dy)
    {
        swap(x1,x2,y1,y2);
        int tmp=dx;
        dx=dy;
        dy=tmp;
        p=1;
    }
    int d=(2*dy)-dx;

    int change1=2*(dy-dx);
    int change2=2*dy;

    p?SetPixel(hdc,y1,x1,color):SetPixel(hdc,x1,y1,color);

    for(int i=0;i<=dx;i++)
    {
        x1 < x2 ? x1++ : x1--;
        if(d<0)
        {
            d+=change2;
        }
        else
        {
            y1 < y2 ? y1++ : y1--;
            d+=change1;
        }
        p?SetPixel(hdc,y1,x1,color):SetPixel(hdc,x1,y1,color);
    }
}
void parm(HDC hdc,int x1,int y1,int x2,int y2,COLORREF color)
{
    double t=0;

    for ( t=0.0;t<=1;t=t+0.001)
    {
       double x=x1+t*(x2-x1);
       double y=y1+t*(y2-y1);
       SetPixel(hdc,round(x),round(y),color);
    }
}

void Draw8Points(HDC hdc,int xc,int yc, int a, int b,COLORREF color)
{

    SetPixel(hdc, xc+a, yc+b, color);
    SetPixel(hdc, xc-a, yc+b, color);
    SetPixel(hdc, xc-a, yc-b, color);
    SetPixel(hdc, xc+a, yc-b, color);
    SetPixel(hdc, xc+b, yc+a, color);
    SetPixel(hdc, xc-b, yc+a, color);
    SetPixel(hdc, xc-b, yc-a, color);
    SetPixel(hdc, xc+b, yc-a, color);

}
void circleDirect(HDC hdc, int xc, int yc, int R,COLORREF color) {
    int x = 0;
    double y = R;
    Draw8Points(hdc, xc, yc, x, y,color);
    while (x < y) {
		x++;
		y = sqrt((double)R * R - x * x);
        Draw8Points(hdc, xc, yc, x, y,color);
    }
    cout<<"Circle Drawn Successfully with radius : "<<R<<endl;
}
void CirclePolar(HDC hdc,int xc,int yc, int R,COLORREF color)
{

    int x=R,y=0;
    double theta=0,dtheta=1.0/R;
    Draw8Points(hdc,xc,yc,x,y,color);
    while(x>y)
    {
        theta+=dtheta;
        x=R*cos(theta);
        y=R*sin(theta);
        Draw8Points(hdc,xc,yc,round(x),round(y),color);
    }

    cout<<"Circle Drawn Successfully with radius : "<<R<<endl;
}
void CircleIterative(HDC hdc,int xc,int yc, int R,COLORREF color)
{
    double x=R,y=0;
    double x1=0;
    double dtheta=1.0/R;
    double ct=cos(dtheta);
    double st=sin(dtheta);
    Draw8Points(hdc,xc,yc,x,y,color);
    while(x>y)
    {
        x1=(x*ct)-(y*st);
        y=(x*st)+(y*ct);
        x=x1;
        Draw8Points(hdc,xc,yc,round(x),round(y),color);
    }

    cout<<"Circle Drawn Successfully with radius : "<<R<<endl;
}
void CircleMidpoint(HDC hdc, int xc, int yc, int r,COLORREF color)
{
    int x = 0;
    int y = r;
    double d = 1 - r;
    Draw8Points(hdc, xc, yc, x, y,color);
    while (x < y)
    {
        if (d <= 0) {
            d = d + 2 * x + 3;
            x++;
        }
        else {
            d = d + 2 * (x - y) + 5;
            x++;
            y--;
        }
        Draw8Points(hdc, xc, yc, x, y,color);
    }

    cout<<"Circle Drawn Successfully with radius : "<<r<<endl;
}
void CircleModifiedMidpoint(HDC hdc, int xc, int yc, int R,COLORREF color)
{
    int x = 0, y = R;
    int d = 1 - R;
    int c1 = 3, c2 = 5 - 2 * R;
    Draw8Points(hdc, xc, yc, x, y,color);
    while (x < y)
    {
        if (d < 0)
        {
            d += c1;
            c2 += 2;
        }
        else
        {
            d += c2;
            c2 += 4;
            y--;
        }
        c1 += 2;
        x++;
        Draw8Points(hdc, xc, yc, x, y,color);
    }

    cout<<"Circle Drawn Successfully with radius : "<<R<<endl;
}
void Draw4Points(HDC hdc,int xc,int yc, int a, int b,COLORREF color)
{

    SetPixel(hdc, xc+a, yc+b, color);
    SetPixel(hdc, xc-a, yc+b, color);
    SetPixel(hdc, xc-a, yc-b, color);
    SetPixel(hdc, xc+a, yc-b, color);
}
void EllipseDirect(HDC hdc, int xc,int yc,int a,int b,COLORREF color){

    int x = 0, y = b;
    Draw4Points(hdc,xc,yc,x,y,color);
    while (x*(b*b) < ((a*a)*y))
    {
        x++;
        y =round(sqrt(((b*b)*((a*a)-(x*x)))/(a*a)));
        Draw4Points(hdc,xc,yc,x,y,color);
    }
    y = 0, x = a;
        Draw4Points(hdc,xc,yc,x,y,color);
    while (x*(b*b) >((a*a)*y))
    {
        y++;
        x = round(sqrt(((a*a)*((b*b) - (y * y)))/(b*b)));
        Draw4Points(hdc,xc,yc,x,y,color);
    }
}
void EllipsePolar(HDC hdc,int xc,int yc, int R1,int R2,COLORREF color){
    int x=R1,y=0;
    double theta=0,dtheta=1.0/R1;
    Draw4Points(hdc,xc,yc,x,y,color);
    while(x+R1>y)
    {
        theta+=dtheta;
        x=(R1*cos(theta));
        y=(R2*sin(theta));
        Draw4Points(hdc,xc,yc,x,y,color);
    }

}
void ELlipseMidPoint(HDC hdc,int xc,int yc, int r1,int r2,COLORREF color)
{
    double dx, dy, d1, d2, x, y;
    x = 0;
    y = r2;
    d1 = (r2 * r2) - (r1 * r1 * r2) + (0.25 * r1 * r1);
    dx = 2 * r2 * r2 * x;
    dy = 2 * r1 * r1 * y;
    while (dx < dy)
    {
        Draw4Points(hdc,xc,yc,x,y,color);
        if (d1 < 0)
        {
            x++;
            dx += (2 * r2 * r2);
            d1 += + dx + (r2 * r2);
        }
        else
        {
            x++;
            y--;
            dx+= (2 * r2 * r2);
            dy -= (2 * r1 * r1);
            d1 += dx - dy + (r2 * r2);
        }
    }

    d2 = ((r2 * r2) * ((x + 0.5) * (x + 0.5))) +
         ((r1 * r1) * ((y - 1) * (y - 1))) -
          (r1 * r1 * r2 * r2);
    while (y >= 0)
    {
        Draw4Points(hdc,xc,yc,x,y,color);
        if (d2 > 0)
        {
            y--;
            dy -= (2 * r1 * r1);
            d2 += (r1 * r1) - dy;
        }
        else
        {
            y--;
            x++;
            dx += (2 * r2 * r2);
            dy -= (2 * r1 * r1);
            d2 += dx - dy + (r1 * r1);
        }
    }
}


void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c,int q)
{
        if(q==1)
        {
            x2 = x1+x2;
            y2 = y1-y2;
        }
        else if(q==2)
        {
            x2 = x1-x2;
            y2 = y1-y2;
        }
        else if(q==3)
        {
            x2 = x1-x2;
            y2 = y1+y2;
        }
        else{
            x2 = x1+x2;
            y2 = y1+y2;
        }
    if(x1>x2 || y1>y2)
    {
        swap(x1,y1,x2,y2);
    }
    int dx = x2 - x1;
    int dy = y2 - y1;
    double constant = (dy>dx?1.0/dy:1.0/dx),x,y;
    for(double t=0;t<1;t+=constant)
    {
        x= x1+t*(x2-x1);
        y= y1+t*(y2-y1);
        SetPixel(hdc, Round(x), Round(y),c);
    }
}

void filingWithLine(HDC hdc, int xc,int yc,int r,int q,COLORREF c)
{
    double x=r,y=0;
    double dt=1.0/r;
    double cdt = cos(dt), sdt = sin(dt);
    Draw8Points(hdc,xc,yc,x,y,c);
    DrawLine(hdc,xc,yc,Round(x),Round(y),c,q);
    DrawLine(hdc,xc,yc,Round(y),Round(x),c,q);
    while(x>y)
    {
        double x1 = x*cdt-y*sdt;
        y = x*sdt+y*cdt;
        x=x1;
        Draw8Points(hdc,xc,yc,Round(x),Round(y),c);
        DrawLine(hdc,xc,yc,Round(x),Round(y),c,q);
        DrawLine(hdc,xc,yc,Round(y),Round(x),c,q);
    }
}
void drawQuarter(HDC hdc, int x, int y, int xc, int yc , int quarter,COLORREF c) {
        switch(quarter){
            case 4:
                SetPixel(hdc, xc + x, yc + y, c);
                SetPixel(hdc, xc + y, yc + x,c);

                break;
            case 1 :
                SetPixel(hdc, xc + y, yc - x,c);
                SetPixel(hdc, xc + x, yc - y,c);

                break;
            case 3 :
                 SetPixel(hdc, xc - x, yc + y,c);
                SetPixel(hdc, xc - y, yc + x,c);
                 break;
            case 2:
                SetPixel(hdc, xc - x, yc - y,c);
                SetPixel(hdc, xc - y, yc - x,c);
                break;
            default:
                break;
}
}
void filingWithCircle(HDC hdc, int xc, int yc, int r,int quarter,COLORREF c) {
    int x = 0;
    int y = r;
    double d = 1 - r;
    drawQuarter(hdc, x, y, xc, yc,quarter,c);
    while (x < y) {
        if (d <= 0) {
            d += (2 * x + 3);
            x++;
        }
        else {
            d += 2 * (x - y) + 5;
            x++;
            y--;
        }
        drawQuarter(hdc, x, y, xc, yc,quarter,c);
    }
}

union OutCode
{
    unsigned All:4;
    struct
    {
        unsigned left:1,top:1,right:1,bottom:1;
    };
};
OutCode GetOutCode(double x,double y,int xleft,int ytop,int xright,int ybottom)
{
    OutCode out;
    out.All=0;
    if(x<xleft)
        out.left=1;
    else if(x>xright)
        out.right=1;
    if(y<ytop)
        out.top=1;
    else if(y>ybottom)
        out.bottom=1;
    return out;
}
void VIntersect(double xs,double ys,double xe,double ye,int x,double *xi,double *yi)
{
    *xi=x;
    *yi=ys+(x-xs)*(ye-ys)/(xe-xs);
}
void HIntersect(double xs,double ys,double xe,double ye,int y,double *xi,double *yi)
{
    *yi=y;
    *xi=xs+(y-ys)*(xe-xs)/(ye-ys);
}
void CohenSuth(HDC hdc,int xs,int ys,int xe,int ye,int xleft,int ytop,int xright,int ybottom,COLORREF color)
{
    double x1=xs,y1=ys,x2=xe,y2=ye;
    OutCode out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
    OutCode out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
    while( (out1.All || out2.All) && !(out1.All & out2.All))
    {
        double xi,yi;
        if(out1.All)
        {
            if(out1.left)
                VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
            else if(out1.top)
                HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out1.right)
                VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
            else
                HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
            x1=xi;
            y1=yi;
            out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
        }
        else
        {
            if(out2.left)
                VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
            else if(out2.top)
                HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out2.right)
                VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
            else
                HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
            x2=xi;
            y2=yi;
            out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
        }
    }
    if(!out1.All && !out2.All)
    {
        LineDDA(hdc, x1, y1, x2, y2, color);
    }
}
struct Vertex
{
    double x,y;
    Vertex(int x1=0,int y1=0)
    {
    x=x1;
    y=y1;
    }
};
typedef std::vector <Vertex> VertexList;
typedef bool (*IsInFunc)(Vertex& v,int edge);
typedef Vertex (*IntersectFunc)(Vertex& v1,Vertex& v2,int edge);
VertexList ClipWithEdge(VertexList p,int edge,IsInFunc In,IntersectFunc Intersect)
{
    VertexList OutList;
    Vertex v1=p[p.size()-1];
    bool v1_in=In(v1,edge);
    for(int i=0;i<(int)p.size();i++)
    {
        Vertex v2=p[i];
        bool v2_in=In(v2,edge);
        if(!v1_in && v2_in)
        {
            OutList.push_back(Intersect(v1,v2,edge));
            OutList.push_back(v2);
        }else if(v1_in && v2_in) OutList.push_back(v2);
        else if(v1_in) OutList.push_back(Intersect(v1,v2,edge));
        v1=v2;
        v1_in=v2_in;
    }
    return OutList;
}

bool InLeft(Vertex& v,int edge)
{
return v.x>=edge;
}
bool InRight(Vertex& v,int edge)
{
return v.x<=edge;
}
bool InTop(Vertex& v,int edge)
{
return v.y>=edge;
}
bool InBottom(Vertex& v,int edge)
{
return v.y<=edge;
}

Vertex VIntersect(Vertex& v1,Vertex& v2,int xedge)
{
    Vertex res;
    res.x=xedge;
    res.y=v1.y+(xedge-v1.x)*(v2.y-v1.y)/(v2.x-v1.x);
    return res;
}
Vertex HIntersect(Vertex& v1,Vertex& v2,int yedge)
{
    Vertex res;
    res.y=yedge;
    res.x=v1.x+(yedge-v1.y)*(v2.x-v1.x)/(v2.y-v1.y);
    return res;
}

void PolygonClip(HDC hdc,POINT *p,int n,int xleft,int ytop,int xright,int ybottom,COLORREF color)
{
    VertexList vlist;
    for(int i=0;i<n;i++)vlist.push_back(Vertex(p[i].x,p[i].y));
    vlist=ClipWithEdge(vlist,xleft,InLeft,VIntersect);
    vlist=ClipWithEdge(vlist,ytop,InTop,HIntersect);
    vlist=ClipWithEdge(vlist,xright,InRight,VIntersect);
    vlist=ClipWithEdge(vlist,ybottom,InBottom,HIntersect);
    Vertex v1=vlist[vlist.size()-1];
    for(int i=0;i<(int)vlist.size();i++)
    {
        Vertex v2=vlist[i];
        LineDDA(hdc,Round(v1.x),Round(v1.y),Round(v2.x),Round(v2.y),color);
        v1=v2;
    }
}
void PointClipping(HDC hdc, int x,int y,int xleft,int ytop,int xright,int ybottom,COLORREF color)
{
    if(x>=xleft && x<= xright && y>=ytop && y<=ybottom)

    SetPixel(hdc,x,y,color);
}
void drawRectangle(HDC hdc,int x1, int y1,int x2,int y2,COLORREF color)
{
    LineDDA(hdc, x1, y1, x2, y1, color);
    LineDDA(hdc, x1, y1, x1, y2, color);
    LineDDA(hdc, x1, y2, x2, y2, color);
    LineDDA(hdc, x2, y1, x2, y2, color);
}
void drawSquare(HDC hdc,int x1, int y1, double sidelength , COLORREF color)
{
    double halfside = sidelength;
    LineDDA(hdc,x1 + halfside, y1 - halfside,x1 + halfside, y1 + halfside,color);
    LineDDA(hdc,x1 - halfside, y1 - halfside,x1 + halfside, y1 - halfside,color);
    LineDDA(hdc,x1 - halfside, y1 + halfside,x1 - halfside, y1 - halfside,color);
    LineDDA(hdc,x1 + halfside, y1 + halfside,x1 - halfside, y1 + halfside,color);
}
void DrawPoint(HDC hdc, int x,int y, int xc,int yc,int r, COLORREF c)
{
        int Distance = sqrt(pow(x-xc,2)+pow(y-yc,2));
		if(Distance<=r)
        {
            SetPixel(hdc, x,y, c);
        }
}

void clipLine(HDC hdc, int x1, int y1, int x2, int y2,int xc,int yc,int r, COLORREF c)
{
    if(x1>x2 || y1>y2)
    {
        swap(x1,y1,x2,y2);
    }
    int dx = x2 - x1;
    int dy = y2 - y1;
    double x,y;
    for(double t=0;t<1;t+=0.0001)
    {
        x= x1+t*dx;
        y= y1+t*dy;
        DrawPoint(hdc,Round(x),Round(y),xc,yc,r,c);
    }
}

struct Vector{
double v[2];

 Vector(double x=0,double y=0){v[0]=x; v[1]=y;}
 double& operator [] (int i){return v[i];}

};
void DrawBezierCurve(HDC hdc,Vector& p1, Vector& p2, Vector& p3, Vector& p4,COLORREF c,int y1,int y2)
{
    double a0=p1[0],
    a1=(-3*p1[0])+(3*p2[0]),
    a2=(3*p1[0])-(6*p2[0])+(3*p3[0]),
    a3=-p1[0]+(3*p2[0])-(3*p3[0])+p4[0];

    double b0=p1[1],
    b1=(-3*p1[1])+(3*p2[1]),
    b2=(3*p1[1])-(6*p2[1])+(3*p3[1]),
    b3=-p1[1]+(3*p2[1])-(3*p3[1])+p4[1];

    for (double t = 0; t <= 1; t += 0.001)
	{
		double t2 = t*t, t3 = t2*t;
		double x = a0 + a1*t + a2*t2 + a3*t3;
		double y = b0 + b1*t + b2*t2 + b3*t3;
		if(y>=y1 && y<=y2)
            SetPixel(hdc, Round(x), Round(y), c);
	}
}
void drawHermite(HDC hdc,Vector& p1,Vector& t1,Vector& p2,Vector& t2,COLORREF c)
{
    double a0=p1[0];
    double a1=t1[0];
    double a2= -3 * p1[0] - 2 * t1[0] + 3 * p2[0] -t2[0];
    double a3=  2 * p1[0] + t1[0] - 2 * p2[0] +t2[0];
    double b0=p1[1];
    double b1=t1[1];
    double b2=-3 *p1[1]-2* t1[1]+ 3 *p2[1]-t2[1];
    double b3= 2 *p1[1] + t1[1]- 2 * p2[1]+t2[1];
    for (double t=0 ; t<1 ; t+=0.0001)
    {
        double t2=t*t,t3=t*t*t;
        double x=a0+a1*t+a2*t2+a3*t3;
        double y=b0+b1*t+b2*t2+b3*t3;
        SetPixel(hdc,round(x),round(y),c);
    }
}
void cardinalSpline(HDC hdc,int n,Vector *p,int co,COLORREF c){
    Vector t[n];
    for(int i=1 ;i<n-1 ;i++)
        t[i]=(co/2)*(p[i+1][0]-p[i-1][0]);
        t[0]=(co/2)*(p[1][0]-p[0][0]);
        t[n-1]=(co/2)*(p[n-1][0]-p[n-2][0]);
        for(int i=0;i<n-1;i++)
        {
           drawHermite(hdc,p[i],t[i],p[i+1],t[i+1],c);
        }
}
void FillRectangle(HDC hdc,int x1, int y1,int x2,int y2,COLORREF color)
{
    Vector v[4];
    LineDDA(hdc, x1, y1, x2, y1, color);

    LineDDA(hdc, x1, y1, x1, y2, color);

    LineDDA(hdc, x1, y2, x2, y2, color);

    LineDDA(hdc, x2, y1, x2, y2, color);
    if(y1>y2)
        swap(x1,y1,x2,y2);

    int ys=y1+1-75,ye=y2+75;
    while(ys<ye)
    {
        v[0][0] = x1+1;
        v[0][1] = ys;

        v[1][0] = x1+((x2-x1)/4);
        v[1][1] = ys+25;

        v[2][0] = x1+((x2-x1)/4)*3;
        v[2][1] = ys+50;

        v[3][0] = x2-1;
        v[3][1] = ys+75;

        DrawBezierCurve(hdc,v[0],v[1],v[2],v[3],color,y1,y2);
        ys++;
    }
}
void drawHermiteforS(HDC hdc,Vector& p1,Vector& t1,Vector& p2,Vector& t2,COLORREF c,int x1,int x2)
{
    double a0=p1[0];
    double a1=t1[0];
    double a2= -3 * p1[0] - 2 * t1[0] + 3 * p2[0] -t2[0];
    double a3=  2 * p1[0] + t1[0] - 2 * p2[0] +t2[0];
    double b0=p1[1];
    double b1=t1[1];
    double b2=-3 *p1[1]-2* t1[1]+ 3 *p2[1]-t2[1];
    double b3= 2 *p1[1] + t1[1]- 2 * p2[1]+t2[1];
    for (double t=0 ; t<1 ; t+=0.0001)
    {
        double t2=t*t,t3=t*t*t;
        double x=a0+a1*t+a2*t2+a3*t3;
        double y=b0+b1*t+b2*t2+b3*t3;
        if(x>=x1 && x<=x2)
            SetPixel(hdc,round(x),round(y),c);
    }
}
void fillSquare(HDC hdc,int x1, int y1, double sidelength ,COLORREF color)
{
    double halfside = sidelength;
    Vector v[4];
    LineDDA(hdc,x1 + halfside, y1 - halfside,x1 + halfside, y1 + halfside,color);

    LineDDA(hdc,x1 - halfside, y1 - halfside,x1 + halfside, y1 - halfside,color);

    LineDDA(hdc,x1 - halfside, y1 + halfside,x1 - halfside, y1 - halfside,color);

    LineDDA(hdc,x1 + halfside, y1 + halfside,x1 - halfside, y1 + halfside,color);

    int xs=x1-halfside-75,xe=x1+halfside+75;
    while(xs<xe)
    {
        v[0][0] = xs;
        v[0][1] = y1-halfside+1;

        v[1][0] = xs+25;
        v[1][1] = y1-(halfside/2);

        v[2][0] = xs+50;
        v[2][1] = y1+(halfside/2);

        v[3][0] = xs+75;
        v[3][1] = y1+halfside-1;

        Vector T1(3 * (v[1][0] - v[0][0]), 3 * (v[1][1] - v[0][1]));
        Vector T2(3 * (v[3][0] - v[2][0]), 3 * (v[3][1] - v[2][1]));

        drawHermiteforS(hdc,v[0],T1,v[3],T2,color,x1-halfside,x1+halfside);
        xs++;
    }
}
struct Entry
{
    int xmin,xmax;
};

void InitEntries(Entry table[])
{
    for(int i=0; i < MAXENTRIES; i++)
        {
            table[i].xmin =  INT_MAX;
            table[i].xmax = - INT_MAX;
        }
}
void ScanEdge(POINT v1,POINT v2,Entry table[])
{
    if(v1.y==v2.y)return;
    if(v1.y > v2.y)swap(v1,v2);
    double minv=(double)(v2.x-v1.x)/(v2.y-v1.y);
    double x=v1.x;
    int y=v1.y;
    while(y<v2.y){
            if(x<table[y].xmin)table[y].xmin=(int)ceil(x);
            if(x>table[y].xmax)table[y].xmax=(int)floor(x);
            y++;
            x+=minv;
    }
}
void DrawSanLines(HDC hdc,Entry table[],COLORREF color)
{
    for(int y=0;y<MAXENTRIES;y++)
    if(table[y].xmin<table[y].xmax)
    for(int x=table[y].xmin;x<=table[y].xmax;x++)
    SetPixel(hdc,x,y,color);
}
void ConvexFill(HDC hdc,POINT p[],int n,COLORREF color)
{
    Entry *table=new Entry[MAXENTRIES];
    InitEntries(table);
    POINT v1=p[n-1];
    for(int i=0;i<n;i++){
            POINT v2=p[i];
            ScanEdge(v1,v2,table);
            v1=p[i];
            }
    DrawSanLines(hdc,table,color);
    delete table;
}
//general

struct EdgeRec
{
    double x;
    double minv;
    int ymax;
    bool operator<(EdgeRec r)
    {
    return x<r.x;
    }
};
typedef list<EdgeRec> EdgeList;

EdgeRec InitEdgeRec(POINT& v1,POINT& v2)
{
    if(v1.y>v2.y)swap(v1,v2);
    EdgeRec rec;
    rec.x=v1.x;
    rec.ymax=v2.y;
    rec.minv=(double)(v2.x-v1.x)/(v2.y-v1.y);
    return rec;
}
void InitEdgeTable(POINT *polygon,int n,EdgeList table[])
{
    POINT v1=polygon[n-1];
    for(int i=0;i<n;i++)
    {
        POINT v2=polygon[i];
        if(v1.y==v2.y){v1=v2;continue;}
        EdgeRec rec=InitEdgeRec(v1, v2);
        table[v1.y].push_back(rec);
        v1=polygon[i];
    }
}
void GeneralPolygonFill(HDC hdc,POINT *polygon,int n,COLORREF c)
{
    EdgeList *table=new EdgeList [MAXENTRIES];
    InitEdgeTable(polygon,n,table);
    int y=0;
    while(y<MAXENTRIES && table[y].size()==0)y++;
    if(y==MAXENTRIES)return;
    EdgeList ActiveList=table[y];
    while (ActiveList.size()>0)
    {
        ActiveList.sort();
        for(EdgeList::iterator it=ActiveList.begin();it!=ActiveList.end();it++)
        {
            int x1=(int)ceil(it->x);
            it++;
            int x2=(int)floor(it->x);
            for(int x=x1;x<=x2;x++)SetPixel(hdc,x,y,c);
        }
        y++;
        EdgeList::iterator it=ActiveList.begin();
        while(it!=ActiveList.end())
        if(y==it->ymax) it=ActiveList.erase(it); else it++;
        for(EdgeList::iterator it=ActiveList.begin();it!=ActiveList.end();it++)
        it->x+=it->minv;
        ActiveList.insert(ActiveList.end(),table[y].begin(),table[y].end());
    }
    delete[] table;
}
void fillShape(HDC hdc,int x,int y ,COLORREF c,COLORREF f)
{
    COLORREF tmp =GetPixel(hdc,x,y);
    if(tmp == c||tmp==f)
        return;
    SetPixel(hdc,x,y,f);
    fillShape(hdc,x+1,y,c,f);
    fillShape(hdc,x,y+1,c,f);
    fillShape(hdc,x-1,y,c,f);
    fillShape(hdc,x,y-1,c,f);
}
struct vertex
{
int x,y;
vertex(int x,int y):x(x),y(y)
{
}
};
void fillShape2(HDC hdc,int x,int y ,COLORREF c,COLORREF f)
{
    stack<vertex> s;
    s.push(vertex(x,y));
    while(! s.empty())
    {
        vertex p=s.top();
        s.pop();
        COLORREF tmp =GetPixel(hdc,p.x,p.y);
        if(tmp == c || tmp==f)
            continue;
        SetPixel(hdc,p.x,p.y,f);
        s.push(vertex(p.x+1,p.y));
        s.push(vertex(p.x,p.y+1));
        s.push(vertex(p.x-1,p.y));
        s.push(vertex(p.x,p.y-1));
    }
}

/*  This function is called by the Windows function DispatchMessage()  */

int xc,yc,x,y,x2,y2,X_start,X_end,Y_start,Y_end,X_left,Y_top,X_right,Y_bottom,R,rad[2],pic=0;
static int index = 0,Num_of_Points=0,p1=0,p2=0,p3=0,p4=0,sideLength=75;
POINT P[5];
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc = GetDC(hwnd);
    static int choose;
    static int color=RGB(0,0,0);
    switch (message)                  /* handle the messages */
    {
    static Vector points[10];
    case WM_COMMAND:
        if (wParam==12){
            color=RGB(255,0,0);
         }
        else if(wParam==13){
             color=RGB(0,255,0);
        }
        else if(wParam==14){
            color=RGB(0,0,255);
        }
        else if(wParam==15){
            color=RGB(255,255,255);
        }
         else if(wParam==155){
            color=RGB(0,0,0);
        }
        else if(wParam==16){
            filingWithLine(hdc,xc,yc,R,1,color);
        }
        else if(wParam==17){

            filingWithLine(hdc,xc,yc,R,2,color);
        }
        else if(wParam==18){

            filingWithLine(hdc,xc,yc,R,3,color);
        }
        else if(wParam==19){

            filingWithLine(hdc,xc,yc,R,4,color);

        }
        else if(wParam==20){
            for (int i = 0; i < R; i++) {
                filingWithCircle(hdc,xc,yc,i,1,color);
            }
        }
        else if(wParam==21){
            for (int i = 0; i < R; i++) {
                filingWithCircle(hdc,xc,yc,i,2,color);
            }
        }
        else if(wParam==22){
            for (int i = 0; i < R; i++) {
                filingWithCircle(hdc,xc,yc,i,3,color);
            }
        }
        else if(wParam==23){
            for (int i = 0; i < R; i++) {
                filingWithCircle(hdc,xc,yc,i,4,color);
            }
        }
        else if(wParam==100){
            string str = "pic";
            str+=to_string (pic);
            str+=".bmp";
            const char * c = str.c_str();
            save(c, hdc, {0, 0, 1366, 768});
            ReleaseDC(hwnd, hdc);
            pic++;
        }
        else if(wParam==200){
            load( hwnd,hdc);
        }
        else if(wParam==300){
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else
            choose=wParam;
        break;
    case WM_LBUTTONUP:
        xc = LOWORD(lParam);
        yc = HIWORD(lParam);
        break;
    case WM_RBUTTONUP:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        if(choose==1){
           LineDDA(hdc,xc,yc,x,y,color);
        }
        else if(choose==2){
            Midpoint(hdc,xc,yc,x,y,color);
        }
        else if(choose==3){
            parm(hdc,xc,yc,x,y,color);
        }
        if(choose==4){
            R = sqrt(pow((x-xc), 2) + pow((y-yc), 2));
            circleDirect(hdc,xc,yc,R,color);
        }
        else if(choose==5){
            R = sqrt(pow((x-xc), 2) + pow((y-yc), 2));
            CirclePolar(hdc,xc,yc,R,color);
        }
        else if(choose==6){
            R = sqrt(pow((x-xc), 2) + pow((y-yc), 2));
            CircleIterative(hdc,xc,yc,R,color);
        }
        else if(choose==7){
            R = sqrt(pow((x-xc), 2) + pow((y-yc), 2));
            CircleMidpoint(hdc,xc,yc,R,color);
        }
        else if(choose==8){
            R = sqrt(pow((x-xc), 2) + pow((y-yc), 2));
            CircleModifiedMidpoint(hdc,xc,yc,R,color);
        }
        else if(choose==9){
            R = sqrt(pow((x-xc), 2) + pow((y-yc), 2));
            rad[index]=R;
            if (index == 1) {
               ELlipseMidPoint(hdc,  xc, yc,rad[index],rad[index-1],color);
              index=0;
            }
            else
                index++;
        }
        else if(choose==10){
            R = sqrt(pow((x-xc), 2) + pow((y-yc), 2));
            rad[index]=R;
            if (index == 1) {
               ELlipseMidPoint(hdc,  xc, yc,rad[index],rad[index-1],color);
              index=0;
            }
            else
                index++;

        }
        else if(choose==11){
            R = sqrt(pow((x-xc), 2) + pow((y-yc), 2));
            rad[index]=R;
            if (index == 1) {
               ELlipseMidPoint(hdc,  xc, yc,rad[index],rad[index-1],color);
              index=0;
            }
            else
                index++;

        }
        else if(choose==24){
            if(Num_of_Points==0)
            {
                X_start=LOWORD(lParam);
                Y_start=HIWORD(lParam);
                PointClipping(hdc,X_start,Y_start,X_left,Y_top,X_right,Y_bottom, color);
                Num_of_Points=0;
            }

        }
        else if(choose==25){
            if(p3==0)
            {
                X_start=LOWORD(lParam);
                Y_start=HIWORD(lParam);
                p3++;
            }
            else if (p3==1)
            {
                X_end=LOWORD(lParam);
                Y_end=HIWORD(lParam);
                CohenSuth(hdc,X_start,Y_start,X_end,Y_end,X_left,Y_top,X_right,Y_bottom,color);
                p3=0;
            }

        }
        else if(choose==30){

            if(p3==0)
            {
                X_left=LOWORD(lParam);
                Y_top=HIWORD(lParam);
                p3++;
            }
            else if (p3==1)
            {
                X_right=LOWORD(lParam);
                Y_bottom=HIWORD(lParam);
                drawRectangle(hdc,X_left,Y_top,X_right,Y_bottom,color);
                p3=0;
            }

        }
        else if(choose==26){
             if(p3==0)
            {
               P[0].x = LOWORD(lParam);
               P[0].y = HIWORD(lParam);
               p3++;
            }
            else if(p3==1)
            {
               P[1].x=LOWORD(lParam);
               P[1].y=HIWORD(lParam);
               p3++;
            }
            else if(p3==2)
            {
               P[2].x=LOWORD(lParam);
               P[2].y=HIWORD(lParam);
               p3++;
            }
            else if(p3==3)
            {
               P[3].x=LOWORD(lParam);
               P[3].y=HIWORD(lParam);
               p3++;
            }
            else if(p3==4)
            {
               P[4].x=LOWORD(lParam);
               P[4].y=HIWORD(lParam);
               PolygonClip(hdc,P,5, X_left, Y_top, X_right, Y_bottom,color);
               p3=0;

            }

        }
        else if(choose==34){
                X_left=LOWORD(lParam);
                Y_top=HIWORD(lParam);
                drawSquare(hdc,X_left,Y_top,sideLength,color);
        }
        else if(choose==27){
            if(p1==0)
            {
                X_start=LOWORD(lParam);
                Y_start=HIWORD(lParam);
                PointClipping(hdc,X_start,Y_start,X_left-sideLength,Y_top-sideLength,X_left+sideLength,Y_top+sideLength,color);
                p1=0;
            }
            break;
        }
        else if(choose==28){
            if(p1==0)
            {
                X_start=LOWORD(lParam);
                Y_start=HIWORD(lParam);
                p1++;
            }
            else if (p1==1)
            {
                X_end=LOWORD(lParam);
                Y_end=HIWORD(lParam);
                CohenSuth(hdc,X_start,Y_start,X_end,Y_end,X_left-sideLength,Y_top-sideLength,X_left+sideLength,Y_top+sideLength,color);
                p1=0;
            }

        }
        else if(choose==31){
            R = sqrt(pow((x-xc), 2) + pow((y-yc), 2));
            CircleMidpoint(hdc,xc,yc,R,color);
        }
        else if(choose==32){
            DrawPoint(hdc,x,y,xc,yc,R,color);
        }
        else if(choose==33){
            if(p3==0)
            {
                X_left=LOWORD(lParam);
                Y_top=HIWORD(lParam);
                p3++;
            }
            else if (p3==1)
            {
                X_right=LOWORD(lParam);
                Y_bottom=HIWORD(lParam);
                clipLine(hdc,X_left,Y_top,X_right,Y_bottom,xc,yc,R,color);
                p3=0;
            }

        }
        else if(choose==29){
            points[index]=Vector(LOWORD(lParam),HIWORD(lParam));
            if(index==5){
                hdc=GetDC(hwnd);
                cardinalSpline(hdc,6,points,2,color);
                ReleaseDC(hwnd,hdc);
                index=0;
            }
            else
                index++;

        }
        else if(choose==37){
            if(Num_of_Points==0)
            {
               P[0].x = LOWORD(lParam);
               P[0].y = HIWORD(lParam);
               Num_of_Points++;
            }
            else if(Num_of_Points==1)
            {
               P[1].x=LOWORD(lParam);
               P[1].y=HIWORD(lParam);
               Num_of_Points++;
            }
            else if(Num_of_Points==2)
            {
               P[2].x=LOWORD(lParam);
               P[2].y=HIWORD(lParam);
               Num_of_Points++;
            }
            else if(Num_of_Points==3)
            {
               P[3].x=LOWORD(lParam);
               P[3].y=HIWORD(lParam);
               Num_of_Points++;
            }
            else if(Num_of_Points==4)
            {
               P[4].x=LOWORD(lParam);
               P[4].y=HIWORD(lParam);
               Num_of_Points++;
               //change pen color//
               SelectObject(hdc, GetStockObject(DC_PEN));
               SetDCPenColor(hdc, color);
               Polygon(hdc, P, 5);
            }
            else if(Num_of_Points==5)
            {
               ConvexFill(hdc, P, 5,color );
               Num_of_Points=0;
            }
        }
        else if(choose==38){
            if(Num_of_Points==0)
            {
               P[0].x = LOWORD(lParam);
               P[0].y = HIWORD(lParam);
               Num_of_Points++;
            }
            else if(Num_of_Points==1)
            {
               P[1].x=LOWORD(lParam);
               P[1].y=HIWORD(lParam);
               Num_of_Points++;
            }
            else if(Num_of_Points==2)
            {
               P[2].x=LOWORD(lParam);
               P[2].y=HIWORD(lParam);
               Num_of_Points++;
            }
            else if(Num_of_Points==3)
            {
               P[3].x=LOWORD(lParam);
               P[3].y=HIWORD(lParam);
               Num_of_Points++;
            }
            else if(Num_of_Points==4)
            {
               P[4].x=LOWORD(lParam);
               P[4].y=HIWORD(lParam);
               Num_of_Points++;
               //change pen color//
               SelectObject(hdc, GetStockObject(DC_PEN));
               SetDCPenColor(hdc, color);
               Polygon(hdc, P, 5);
            }
            else if(Num_of_Points==5)
            {
               GeneralPolygonFill(hdc, P, 5, color);
               Num_of_Points=0;
            }
        }
        else if(choose==39){
            fillShape( hdc, x, y , color,RGB(255,0,0));
        }
        else if(choose==40){
            fillShape2( hdc, x, y , color,RGB(255,0,0));
        }
        else if(choose==41){
            fillSquare(hdc,x,y,sideLength,color);
        }
        else if(choose==44){
            if(p1==0){
            x2=LOWORD(lParam);
            y2=HIWORD(lParam);
            p1++;
            }
            else{
            FillRectangle(hdc,x,y,x2,y2,color);
            p1=0;
            }
        }
    case WM_CREATE:
	{
        HMENU menu = CreateMenu();
        HMENU file = CreateMenu();
        AppendMenu(menu, MF_POPUP, (UINT_PTR)file, "File");
        AppendMenu(file, MF_STRING,100 ,"save");
        AppendMenu(file, MF_STRING,200 ,"load");
        AppendMenu(file, MF_STRING,300,"clear");

        HMENU line = CreateMenu();
        AppendMenu(menu, MF_POPUP, (UINT_PTR)line, "Line");
        AppendMenu(line, MF_STRING,1 ,"DDA");
        AppendMenu(line, MF_STRING,2 ,"Midpoint");
        AppendMenu(line, MF_STRING,3 ,"Parametric");

        HMENU Circle= CreateMenu();
        AppendMenu(menu, MF_POPUP, (UINT_PTR)Circle, "Circle");
        AppendMenu(Circle, MF_STRING,4 ,"Direct");
        AppendMenu(Circle, MF_STRING,5 ,"Polar");
        AppendMenu(Circle, MF_STRING,6 ,"Iterative Polar");
        AppendMenu(Circle, MF_STRING,7 ,"Midpoint");
        AppendMenu(Circle, MF_STRING,8 ,"Modified Midpoint");

        HMENU ellipse = CreateMenu();
        AppendMenu(menu, MF_POPUP, (UINT_PTR)ellipse, "Ellipse");
        AppendMenu(ellipse, MF_STRING,9 ,"Direct");
        AppendMenu(ellipse, MF_STRING,10 ,"Polar");
        AppendMenu(ellipse, MF_STRING,11 ,"Midpoint");

        HMENU colors = CreateMenu();
        AppendMenu(menu, MF_POPUP, (UINT_PTR)colors, "Color");
        AppendMenu(colors, MF_STRING,12 ,"Red");
        AppendMenu(colors, MF_STRING,13 ,"Green");
        AppendMenu(colors, MF_STRING,14 ,"Blue");
        AppendMenu(colors, MF_STRING,15 ,"White");
        AppendMenu(colors, MF_STRING,155 ,"Black");

        HMENU Filling = CreateMenu();
        HMENU Fillingline = CreateMenu();
        HMENU hFillingcircle = CreateMenu();
        AppendMenu(menu, MF_POPUP, (UINT_PTR)Filling, "Filling");
        AppendMenu(Filling, MF_POPUP, (UINT_PTR)Fillingline, "Filling line");
        AppendMenu(Fillingline, MF_STRING,16 ,"1");
        AppendMenu(Fillingline, MF_STRING,17 ,"2");
        AppendMenu(Fillingline, MF_STRING,18 ,"3");
        AppendMenu(Fillingline, MF_STRING,19 ,"4");
        AppendMenu(Filling, MF_POPUP, (UINT_PTR)hFillingcircle, "Filling Circle");
        AppendMenu(hFillingcircle, MF_STRING,20 ,"1");
        AppendMenu(hFillingcircle, MF_STRING,21 ,"2");
        AppendMenu(hFillingcircle, MF_STRING,22 ,"3");
        AppendMenu(hFillingcircle, MF_STRING,23 ,"4");

        HMENU Clipping = CreateMenu();
        HMENU ClippingRectangle = CreateMenu();
        HMENU ClippingSquare = CreateMenu();
        HMENU ClippingCircle = CreateMenu();
        AppendMenu(menu, MF_POPUP, (UINT_PTR)Clipping, "Clipping");
        AppendMenu(Clipping, MF_POPUP, (UINT_PTR)ClippingRectangle, "Clipping Rectangle");
        AppendMenu(ClippingRectangle, MF_STRING,30 ,"window");
        AppendMenu(ClippingRectangle, MF_STRING,24 ,"point");
        AppendMenu(ClippingRectangle, MF_STRING,25 ,"line");
        AppendMenu(ClippingRectangle, MF_STRING,26 ,"Polygon");
        AppendMenu(Clipping, MF_POPUP, (UINT_PTR)ClippingSquare, "Clipping Square");
        AppendMenu(ClippingSquare, MF_STRING,34 ,"window");
        AppendMenu(ClippingSquare, MF_STRING,27 ,"point");
        AppendMenu(ClippingSquare, MF_STRING,28 ,"line");
        AppendMenu(Clipping, MF_POPUP, (UINT_PTR)ClippingCircle, "Clipping Circle");
        AppendMenu(ClippingCircle, MF_STRING,31 ,"window");
        AppendMenu(ClippingCircle, MF_STRING,32 ,"point");
        AppendMenu(ClippingCircle, MF_STRING,33 ,"line");

        AppendMenu(menu, MF_STRING,29 ,"Cardinal Spline");

        HMENU fillcurve = CreateMenu();
        HMENU fillcurvesquare = CreateMenu();
        HMENU fillcurverectangle = CreateMenu();

        AppendMenu(menu, MF_POPUP, (UINT_PTR)fillcurve, "Filing with curve");
        AppendMenu(fillcurve, MF_POPUP, (UINT_PTR)fillcurvesquare, "Square");
        AppendMenu(fillcurvesquare, MF_STRING,41 ,"Hermite (Vertical)");

        AppendMenu(fillcurve, MF_POPUP, (UINT_PTR)fillcurverectangle, "Rectangle");
        AppendMenu(fillcurverectangle, MF_STRING,44 ,"Bezier (Horizontal)");

        HMENU Polygon = CreateMenu();
        AppendMenu(menu, MF_POPUP, (UINT_PTR)Polygon, "Polygon");
        AppendMenu(Polygon, MF_STRING,37 ,"convex");
        AppendMenu(Polygon, MF_STRING,38 ,"general");

        HMENU FloodFill = CreateMenu();
        AppendMenu(menu, MF_POPUP, (UINT_PTR)FloodFill, "FloodFill");
        AppendMenu(FloodFill, MF_STRING,39 ,"Recursive");
        AppendMenu(FloodFill, MF_STRING,40 ,"Non Recursive");

        SetMenu(hwnd, menu);
		break;
	}

        break;
    case WM_DESTROY:
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
