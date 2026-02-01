#pragma once
#include <Windows.h>
#include <iostream>
inline void setup_console() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONIN$", "r", stdin);
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 500, 500, TRUE);
    SetWindowLong(console, GWL_EXSTYLE, GetWindowLong(console, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(console, 0, (255 * 40) / 100, LWA_ALPHA);
    SetLayeredWindowAttributes(console, 0, (BYTE)(255 * 0.6), LWA_ALPHA);
}