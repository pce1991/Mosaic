
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


void MoveMouse(int32 screenX, int32 screenY) {
    Display *display = Platform->display;
    Window window = Platform->window;

    screenY = (int32)Platform->screenHeight - screenY;

    // @NOTE: the src_window is None, because we want to move the cursor
    // regardless of if it's inside the window
    XWarpPointer(display, None, window, 0, 0, 0, 0, screenX, screenY);
    XFlush(display);
}
