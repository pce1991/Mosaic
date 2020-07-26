
struct DebugLogNode {
    int32 count;
    int32 capacity;
    
    char *buffer;

    DebugLogNode *next;
};

struct LogData {
    char *string;
    int32 length;
    int32 frame;
};

struct DebugLog {
    DynamicArray<LogData> logs;

    DebugLogNode *head;
    DebugLogNode *current;
};
