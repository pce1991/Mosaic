
#include "../types.h"

#define PRINT_MAX_BUFFER_LEN 1024

void Print(const char *fmt, ...) {
    char buffer[PRINT_MAX_BUFFER_LEN];

    va_list args;
    va_start (args, fmt);


    vsnprintf(buffer, PRINT_MAX_BUFFER_LEN, fmt, args);

    OutputDebugString(buffer);
    OutputDebugString("\n");

    {
      HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
      if (stdOut != NULL && stdOut != INVALID_HANDLE_VALUE)
      {
        DWORD written = 0;
        WriteConsoleA(stdOut, buffer, strlen(buffer), &written, NULL);
        WriteConsoleA(stdOut, "\n", 1, &written, NULL);
      }
    }

    va_end(args);
}

void PrintNoLine(const char *fmt, ...) {
    char buffer[PRINT_MAX_BUFFER_LEN];

    va_list args;
    va_start (args, fmt);


    vsnprintf(buffer, PRINT_MAX_BUFFER_LEN, fmt, args);

    OutputDebugString(buffer);

    {
      HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
      if (stdOut != NULL && stdOut != INVALID_HANDLE_VALUE)
      {
        DWORD written = 0;
        WriteConsoleA(stdOut, buffer, strlen(buffer), &written, NULL);
      }
    }

    va_end(args);
}


void WinMoveMouse(HWND window, int32 resX, int32 resY, int32 height) {
    POINT screenPoint;

    // @NOTE: convert from internal render resolution coords (y-up, origin bottom
    // left) to window client coords (y-down, origin top left), accounting for the
    // letterbox scale/offset used when presenting the frame.
    screenPoint.x = (LONG)(Platform->presentOffsetX + resX * Platform->presentScale);
    screenPoint.y = (LONG)(Platform->presentOffsetY + (height - resY) * Platform->presentScale);

    ClientToScreen(window, &screenPoint);

    SetCursorPos(screenPoint.x, (int32)screenPoint.y);
}


void MoveMouse(int32 resX, int32 resY) {
    WinMoveMouse(*Platform->window, resX, resY, Platform->resolutionHeight);
}

