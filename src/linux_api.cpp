
#define PRINT_MAX_BUFFER_LEN 1024
void Print(const char *fmt, ...) {
    const uint32 maxBufferLength = 1024;
    char buffer[maxBufferLength];

    va_list args;
    va_start (args, fmt);

    vsnprintf(buffer, maxBufferLength, fmt, args);

    printf(buffer);
    printf("\n");

    va_end(args);
}

void MoveMouse(int32 screenX, int32 screenY) {
    Display *display = Platform->display;
    Window *window = Platform->window;

    screenY = (int32)Platform->screenHeight - screenY;

    // @NOTE: the src_window is none, because want to move the cursor regardless of if its inside the window
    XWarpPointer(display, None, *window, 0, 0, 0, 0, screenX, screenY);
}
