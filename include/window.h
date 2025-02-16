// include/window.h
#pragma once

#include <windows.h>

// Global variables (defined in window.cpp)
extern HWND g_hWnd;
extern HINSTANCE g_hInstance;
extern int g_ScreenWidth;
extern int g_ScreenHeight;
extern void* g_pPixels;
extern int g_VirtualOffsetX;
extern int g_VirtualOffsetY;

// Functions
bool CreateOverlayWindow(int nCmdShow);
bool SetupWindow(int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void CreateDIB(int width, int height);
void UpdateOverlay(HWND hWnd);

// System Tray Functions
void AddTrayIcon(HWND hWnd);
void RemoveTrayIcon(HWND hWnd);
void ShowContextMenu(HWND hWnd, POINT pt);

// System Tray Menu IDs
#define WM_TRAYICON         (WM_USER + 1)
#define ID_TRAY_EXIT        1001
#define ID_TRAY_PARTICLE_1  1002  // Smoke
#define ID_TRAY_PARTICLE_2  1003  // Stars
#define ID_TRAY_PARTICLE_3  1004  // Fire
#define ID_TRAY_PARTICLE_4  1005  // Sparks
#define ID_TRAY_PARTICLE_5  1006  // Hearts
#define ID_TRAY_PARTICLE_6  1007  // Sword
