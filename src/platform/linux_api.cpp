
#include "../types.h"

#define PRINT_MAX_BUFFER_LEN 1024

void Print(const char *fmt, ...) {
    char buffer[PRINT_MAX_BUFFER_LEN];

    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, PRINT_MAX_BUFFER_LEN, fmt, args);

    printf("%s\n", buffer);

    va_end(args);
}

void PrintNoLine(const char *fmt, ...) {
    char buffer[PRINT_MAX_BUFFER_LEN];

    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, PRINT_MAX_BUFFER_LEN, fmt, args);

    printf("%s", buffer);

    va_end(args);
}


void MoveMouse(int32 resX, int32 resY) {
    Display *display = Platform->display;
    Window window = Platform->window;

    // @NOTE: convert from internal render resolution coords (y-up, origin bottom
    // left) to window client coords (y-down, origin top left), accounting for the
    // letterbox scale/offset used when presenting the frame.
    int32 windowX = (int32)(Platform->presentOffsetX + resX * Platform->presentScale);
    int32 windowY = (int32)(Platform->presentOffsetY + ((real32)Platform->resolutionHeight - resY) * Platform->presentScale);

    // @NOTE: the src_window is None, because we want to move the cursor
    // regardless of if it's inside the window
    XWarpPointer(display, None, window, 0, 0, 0, 0, windowX, windowY);
    XFlush(display);
}
