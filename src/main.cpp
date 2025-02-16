// src/main.cpp

#ifndef _WIN32_WINNT
  // Windows 10 version 1703 is 0x0A00; you can adjust as needed.
  #define _WIN32_WINNT 0x0A00
#endif


#include <windows.h>
#include <chrono>
#include "window.h"       // CreateOverlayWindow, CreateDIB, UpdateOverlay, g_hInstance, g_hWnd, etc.
#include "particles.h"    // SpawnParticlesOnMouseMove, UpdateParticles, DrawParticlesToDIB
#include "utils.h"        // RandomHeartColor (if needed)
#include <shellscalingapi.h> // For SetProcessDpiAwarenessContext, SetProcessDPIAware

// Linker libraries for MSVC (MinGW uses -l flags)
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // Set the DPI awareness early on.
    // For Windows 10 version 1703 and later, attempt to use Per-Monitor Aware V2.
    if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
    {
        // Fallback for older systems:
        SetProcessDPIAware();
    }

    // Set global instance (defined in window.cpp)
    g_hInstance = hInstance;

    // 1) Create the overlay window
    if (!CreateOverlayWindow(nCmdShow)) {
        MessageBox(nullptr, TEXT("Failed to create overlay window."), TEXT("Error"), MB_ICONERROR);
        return 1;
    }

    // 2) Create the 32-bit ARGB DIB (the DIB size is based on our calculated virtual screen bounds)
    CreateDIB(g_ScreenWidth, g_ScreenHeight);

    // 3) Initial update
    UpdateOverlay(g_hWnd);

    // 4) Show the window
    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    // Setup for main loop
    MSG msg;
    g_lastFrameTime = std::chrono::steady_clock::now();

    // Main loop
    while (true) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // Calculate delta time
            auto now = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(now - g_lastFrameTime).count();
            g_lastFrameTime = now;

            // 1) Spawn particles on mouse move
            SpawnParticlesOnMouseMove();

            // 2) Update particle positions, rotations, lifetimes
            UpdateParticles(dt);

            // 3) Draw particles into the DIB.
            // Note: In DrawParticlesToDIB(), we convert global coordinates
            // to overlay coordinates by subtracting g_VirtualOffsetX/Y.
            DrawParticlesToDIB();

            // 4) Update overlay
            UpdateOverlay(g_hWnd);

            // ~60 fps
            Sleep(16);
        }
    }

    return static_cast<int>(msg.wParam);
}
