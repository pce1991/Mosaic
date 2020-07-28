
#define LOG_BUFFER_CAPACITY Megabytes(8)

void AllocateDebugLogNode(DebugLogNode *node, int32 capacity) {
    node->count = 0;
    node->capacity = capacity;
    node->buffer = (char *)malloc(capacity);

    node->next = NULL;
}

void Log(char *fmt, ...) {
    DebugLog *log = &Game->log;
    DebugLogNode *buffer = log->current;
    
    va_list args;
    va_start (args, fmt);
    
    char str[512];
    vsnprintf(str, 512, fmt, args);

    int32 len = strlen(str);

    ASSERT(len < buffer->capacity);

    if (len > buffer->count) {
        buffer->next = (DebugLogNode *)malloc(sizeof(DebugLogNode));
        AllocateDebugLogNode(buffer->next, LOG_BUFFER_CAPACITY);

        log->current = buffer->next;
        buffer = log->current;
    }

    char *start = &buffer->buffer[buffer->count];
    memcpy(start, str, len);

    buffer->count += len;

    LogData data = {};

    data.string = start;
    data.length = len;
    data.frame = Game->frame;

    PushBack(&log->logs, data);

    va_end(args);
}


void WriteLogToFile(char *path) {
    FILE *file = fopen(path, "w");

    DebugLog *log = &Game->log;

    int32 frame = -1;

    if (file != NULL) {
        for (int i = 0; i < log->logs.count; i++) {
            LogData l = log->logs[i];

            if (l.frame > frame) {
                fprintf(file, "\n================\n");
                fprintf(file, "FRAME %d\n", l.frame);
                fprintf(file, "================\n\n");
                frame = l.frame;
            }

            fwrite(l.string, 1, l.length, file);
            fputc('\n', file);
        }

        fclose(file);
    }
}
