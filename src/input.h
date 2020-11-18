
// @TODO: number keys, arrow keys, modifier keys

// @TODO: dont bother passing Input into the functions!!!!

enum InputID {
    Input_None,

    Input_Escape,
    Input_Return,
    Input_Backspace,

    Input_Z,
    Input_R,
    
    Input_Up,    // W
    Input_Down,  // S
    Input_Left,  // A 
    Input_Right, // D

    Input_UpArrow,
    Input_DownArrow,
    Input_LeftArrow,
    Input_RightArrow,

    Input_Space,

    Input_MouseLeft,
    Input_MouseRight,

    Input_Num1,
    Input_Num2,
    Input_Num3,

    Input_Count,
};

struct InputEvent {
    int32 deviceID;
    InputID input;
    bool release;
};

struct InputDeviceState{
    int32 *framesHeld;
    bool *released;
    bool *pressed;
    real32 *timePressed;
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
