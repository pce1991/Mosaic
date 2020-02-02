
typedef enum {
    Input_None,

    Input_Escape,
    Input_Return,
    Input_Backspace,
    
    Input_Up,
    Input_Down,
    Input_Left,
    Input_Right,

    Input_Space,

    Input_MouseLeft,
    Input_MouseRight,

    Input_Count,
} InputID;

struct InputEvent {
    int32 deviceID;
    InputID input;
    bool release;
};

struct InputDeviceState{
    int32 *framesHeld;
    bool *released;
    bool *pressed;
};

struct InputQueue {
    uint32 capacity;
    uint32 count;
    uint32 size;

    InputEvent *events;

    int32 deviceCount;
    InputDeviceState *deviceStates;

    uint32 charSize;
    uint32 charCount;
    char *inputChars;

    vec2i mousePos;
    vec2 mousePosNorm;
    vec2 mousePosNormSigned;

#if !_WIN32
    sem_t *semaphore;
#endif
};
