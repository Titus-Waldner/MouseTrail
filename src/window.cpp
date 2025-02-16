// src/window.cpp
#include "window.h"
#include "particles.h"     // For SetActiveParticleSystem
#include "resource.h"      // For IDI_APP (make sure this is in your include folder)
#include <shellapi.h>      // For Shell_NotifyIcon, NOTIFYICONDATA
#include <tchar.h>
#include <climits>         // For INT_MAX, INT_MIN
#include <algorithm>       // For std::min, std::max

// Global Variables
HWND g_hWnd           = nullptr;
HINSTANCE g_hInstance = nullptr;
int g_ScreenWidth     = 0;
int g_ScreenHeight    = 0;
int g_VirtualOffsetX  = 0;  // Left-most coordinate across all monitors
int g_VirtualOffsetY  = 0;  // Top-most coordinate across all monitors
void* g_pPixels       = nullptr;  // Pointer to DIB bits

static HBITMAP g_hDib = nullptr; // Local handle for the DIB

//------------------------------------------------------------------
// MonitorEnumProc
// Called by EnumDisplayMonitors to compute the union of all monitor rectangles
//------------------------------------------------------------------
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    RECT* pVirtualRect = reinterpret_cast<RECT*>(dwData);
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfo(hMonitor, &mi))
    {
        pVirtualRect->left   = std::min(pVirtualRect->left, mi.rcMonitor.left);
        pVirtualRect->top    = std::min(pVirtualRect->top, mi.rcMonitor.top);
        pVirtualRect->right  = std::max(pVirtualRect->right, mi.rcMonitor.right);
        pVirtualRect->bottom = std::max(pVirtualRect->bottom, mi.rcMonitor.bottom);
    }
    return TRUE;
}

//------------------------------------------------------------------
// CalculateVirtualScreenBounds
// Enumerates all monitors and computes the union rectangle
//------------------------------------------------------------------
void CalculateVirtualScreenBounds()
{
    RECT virtualRect;
    virtualRect.left   = INT_MAX;
    virtualRect.top    = INT_MAX;
    virtualRect.right  = INT_MIN;
    virtualRect.bottom = INT_MIN;
    
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&virtualRect));
    
    g_VirtualOffsetX = virtualRect.left;
    g_VirtualOffsetY = virtualRect.top;
    g_ScreenWidth    = virtualRect.right - virtualRect.left;
    g_ScreenHeight   = virtualRect.bottom - virtualRect.top;
}

//------------------------------------------------------------------
// CreateOverlayWindow
//------------------------------------------------------------------
bool CreateOverlayWindow(int nCmdShow)
{
    // Instead of using GetSystemMetrics, calculate the union of all monitors.
    CalculateVirtualScreenBounds();

    // Use WNDCLASSEX so we can set both hIcon and hIconSm.
    WNDCLASSEX wc = {};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = g_hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);

    // Load the icon from your resources.
    wc.hIcon   = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.hIconSm = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP));

    wc.hbrBackground = nullptr;
    wc.lpszClassName = TEXT("HeartsOverlayClass");

    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, TEXT("RegisterClassEx failed!"), TEXT("Error"), MB_ICONERROR);
        return false;
    }

    return SetupWindow(nCmdShow);
}

//------------------------------------------------------------------
// SetupWindow
//------------------------------------------------------------------
bool SetupWindow(int nCmdShow)
{
    // Use WS_EX_TOOLWINDOW to keep the window hidden from the taskbar/Alt+Tab.
    DWORD dwExStyle = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;
    DWORD dwStyle   = WS_POPUP;

    // Create the window at the virtual screen’s top-left.
    g_hWnd = CreateWindowEx(
        dwExStyle,
        TEXT("HeartsOverlayClass"),
        TEXT("Hearts Overlay"),
        dwStyle,
        g_VirtualOffsetX, g_VirtualOffsetY,       // Position using the virtual offset
        g_ScreenWidth, g_ScreenHeight,
        nullptr, nullptr,
        g_hInstance,
        nullptr
    );

    if (!g_hWnd) {
        MessageBox(nullptr, TEXT("CreateWindowEx failed!"), TEXT("Error"), MB_ICONERROR);
        return false;
    }

    // (Optional) Set the window's icon.
    // Although the window will not show in the taskbar,
    // this icon may be used by some system dialogs.
    SendMessage(g_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP)));
    SendMessage(g_hWnd, WM_SETICON, ICON_BIG,   (LPARAM)LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP)));

    // Add the program to the system tray.
    AddTrayIcon(g_hWnd);

    return true;
}

//------------------------------------------------------------------
// AddTrayIcon
//------------------------------------------------------------------
void AddTrayIcon(HWND hWnd)
{
    NOTIFYICONDATA nid = {};
    nid.cbSize           = sizeof(NOTIFYICONDATA);
    nid.hWnd             = hWnd;
    nid.uID              = 1;
    nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    
    // Load the icon from resources.
    nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP));
    lstrcpy(nid.szTip, TEXT("Mouse Trails"));

    Shell_NotifyIcon(NIM_ADD, &nid);
}

//------------------------------------------------------------------
// RemoveTrayIcon
//------------------------------------------------------------------
void RemoveTrayIcon(HWND hWnd)
{
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd   = hWnd;
    nid.uID    = 1;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

//------------------------------------------------------------------
// ShowContextMenu
//------------------------------------------------------------------
void ShowContextMenu(HWND hWnd, POINT pt)
{
    HMENU hMenu = CreatePopupMenu();
    if (hMenu)
    {
        AppendMenu(hMenu, MF_STRING, ID_TRAY_PARTICLE_1, TEXT("Smoke"));
        AppendMenu(hMenu, MF_STRING, ID_TRAY_PARTICLE_2, TEXT("Stars"));
        AppendMenu(hMenu, MF_STRING, ID_TRAY_PARTICLE_3, TEXT("Fire"));
        AppendMenu(hMenu, MF_STRING, ID_TRAY_PARTICLE_4, TEXT("Sparks"));
        AppendMenu(hMenu, MF_STRING, ID_TRAY_PARTICLE_5, TEXT("Hearts"));
        AppendMenu(hMenu, MF_STRING, ID_TRAY_PARTICLE_6, TEXT("Sword"));

        AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));

        SetForegroundWindow(hWnd);
        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
        DestroyMenu(hMenu);
    }
}

//------------------------------------------------------------------
// WndProc
//------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_TRAYICON:
            if (LOWORD(lParam) == WM_RBUTTONDOWN)
            {
                POINT pt;
                GetCursorPos(&pt);
                ShowContextMenu(hWnd, pt);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case ID_TRAY_PARTICLE_1: SetActiveParticleSystem(1); break;
                case ID_TRAY_PARTICLE_2: SetActiveParticleSystem(2); break;
                case ID_TRAY_PARTICLE_3: SetActiveParticleSystem(3); break;
                case ID_TRAY_PARTICLE_4: SetActiveParticleSystem(4); break;
                case ID_TRAY_PARTICLE_5: SetActiveParticleSystem(5); break;
                case ID_TRAY_PARTICLE_6: SetActiveParticleSystem(6); break;
                case ID_TRAY_EXIT:
                    RemoveTrayIcon(hWnd);
                    PostQuitMessage(0);
                    break;
            }
            break;

        case WM_DESTROY:
            RemoveTrayIcon(hWnd);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

//------------------------------------------------------------------
// CreateDIB
//------------------------------------------------------------------
void CreateDIB(int width, int height)
{
    // Release any existing DIB.
    if (g_hDib) {
        DeleteObject(g_hDib);
        g_hDib = nullptr;
    }
    g_pPixels = nullptr;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = width;
    bi.bmiHeader.biHeight      = -height; // top-down DIB
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC hDC = GetDC(nullptr);
    g_hDib = CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, &g_pPixels, nullptr, 0);
    ReleaseDC(nullptr, hDC);
}

//------------------------------------------------------------------
// UpdateOverlay
//------------------------------------------------------------------
void UpdateOverlay(HWND hWnd)
{
    if (!g_hDib || !g_pPixels) return;

    HDC hScreenDC = GetDC(nullptr);
    HDC hMemDC    = CreateCompatibleDC(hScreenDC);
    HBITMAP hOld  = (HBITMAP)SelectObject(hMemDC, g_hDib);

    BLENDFUNCTION blend = {};
    blend.BlendOp             = AC_SRC_OVER;
    blend.BlendFlags          = 0;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat         = AC_SRC_ALPHA;

    // Get the current window position.
    RECT rect;
    GetWindowRect(g_hWnd, &rect);
    POINT dstPos = { rect.left, rect.top };
    SIZE  dstSize = { g_ScreenWidth, g_ScreenHeight };
    POINT srcPos  = { 0, 0 };

    UpdateLayeredWindow(hWnd, hScreenDC, &dstPos, &dstSize, hMemDC, &srcPos, 0, &blend, ULW_ALPHA);

    SelectObject(hMemDC, hOld);
    DeleteDC(hMemDC);
    ReleaseDC(nullptr, hScreenDC);
}
