
#include "types.h"

#define PRINT_MAX_BUFFER_LEN 1024

void Print(const char *fmt, ...) {
    char buffer[PRINT_MAX_BUFFER_LEN];

    va_list args;
    va_start (args, fmt);


    vsnprintf(buffer, PRINT_MAX_BUFFER_LEN, fmt, args);

    OutputDebugString(buffer);
    OutputDebugString("\n");

    va_end(args);
}


void WinMoveMouse(HWND window, int32 screenX, int32 screenY, int32 height) {
    POINT screenPoint;
    screenPoint.x = screenX;
    screenPoint.y = (int32)height - screenY;

    ClientToScreen(window, &screenPoint);

    SetCursorPos(screenPoint.x, (int32)screenPoint.y);
}


void MoveMouse(int32 screenX, int32 screenY) {
    WinMoveMouse(*Platform->window, screenX, screenY, Platform->screenHeight);
}

