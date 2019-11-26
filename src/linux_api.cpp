
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
