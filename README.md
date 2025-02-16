MouseTrail

MouseTrail is a lightweight Windows overlay application that creates a dynamic, particle-based trail effect following your mouse cursor. It supports multiple particle types such as smoke, stars, fire, sparks, hearts, and even a sword effect. The overlay works across multiple monitors, is DPI-aware, and runs without showing a taskbar window (it uses a system tray icon for control).
Features

    Multiple Particle Effects:
    Choose from various particle systems including Smoke, Stars, Fire, Sparks, Hearts, and Sword.

    Multi-Monitor & DPI Awareness:
    The overlay spans across multiple monitors and correctly handles DPI scaling for accurate particle positioning.

    Layered Overlay Window:
    Uses a transparent, click-through layered window to render particle effects on top of your desktop without interfering with other applications.

    System Tray Integration:
    The application is controlled via a system tray icon with a context menu for switching particle effects or exiting the app.

    Customizable Effects:
    Easily add or modify particle effects and behavior in the code.

Requirements

    Operating System: Windows 10 or later
    Compiler: MSVC, MinGW, or any compiler supporting C++17
    Build System: CMake (version 3.10 or later) and Ninja (or your preferred generator)
    Dependencies: Standard Windows libraries (user32, gdi32)
    Optional: A Windows 8.1+ SDK for DPI-awareness APIs

Installation & Building

    Clone the Repository:

git clone https://github.com/yourusername/MouseTrail.git
cd MouseTrail

Prepare the Build Directory:

mkdir build && cd build

Configure with CMake:

cmake -G Ninja ..

Build the Project:

    ninja

    The executable will be generated (e.g., MouseTrail.exe).

Project Structure

MouseTrail/
├── include/
│   ├── resource.h         # Resource header (defines IDI_APP, etc.)
│   └── ...                # Other header files
├── resources/
│   ├── app.rc             # Resource file (embeds app.ico)
│   └── app.ico            # Application icon (includes 16x16, 32x32, 48x48, 256x256 sizes)
├── src/
│   ├── main.cpp           # Entry point (WinMain) with DPI-awareness integration
│   ├── particles.cpp      # Particle system implementation
│   ├── window.cpp         # Window and overlay implementation (multi-monitor, layered window, tray icon)
│   └── utils.cpp          # Utility functions (e.g., RandomHeartColor)
├── CMakeLists.txt         # CMake build configuration
└── README.md              # This file

Usage

    Run the Application:
    Double-click MouseTrail.exe (or run it from the command line). The overlay will start running in the background.

    Control via System Tray:
        Right-click the system tray icon (displaying your custom icon) to bring up the context menu.
        Select a particle effect (e.g., Smoke, Stars, Fire, Sparks, Hearts, Sword) to change the active effect.
        Select Exit to quit the application.

DPI Awareness and Multi-Monitor Support

    The application automatically calculates the union of all monitor rectangles using EnumDisplayMonitors and positions the overlay window accordingly.
    It is marked DPI-aware (via API calls and an embedded manifest, if configured) so that mouse coordinates and particle rendering are scaled correctly on high-DPI displays.

Customization

    Particle Effects:
    Edit particles.cpp to modify existing particle behaviors or add new particle types.

    Window Behavior:
    The overlay window is created as a click-through layered window. To change its behavior (for example, to show/hide from Alt+Tab or the taskbar), modify the window styles in SetupWindow() within window.cpp.

    Icon and Resources:
    Update the icon file in the resources/ folder and modify resource.h/app.rc as needed.