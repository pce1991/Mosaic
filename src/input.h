
// @TODO: dont bother passing Input into the functions!!!!

enum InputDeviceType {
     InputDeviceType_Invalid,
     InputDeviceType_Keyboard,
     InputDeviceType_Mouse,
};

enum InputKeyboardDiscrete : uint32 {
    Input_A,
    Input_B,
    Input_C,
    Input_D,
    Input_E,
    Input_F,
    Input_G,
    Input_H,
    Input_I,
    Input_J,
    Input_K,
    Input_L,
    Input_M,
    Input_N,
    Input_O,
    Input_P,
    Input_Q,
    Input_R,
    Input_S,
    Input_T,
    Input_U,
    Input_V,
    Input_W,
    Input_X,
    Input_Y,
    Input_Z,
    Input_0,
    Input_1,
    Input_2,
    Input_3,
    Input_4,
    Input_5,
    Input_6,
    Input_7,
    Input_8,
    Input_9,
    Input_Equal,
    Input_Minus,
    Input_RightBracket,
    Input_LeftBracket,
    Input_Quote,
    Input_Semicolon,
    Input_Backslash,
    Input_Comma,
    Input_Slash,
    Input_Period,
    Input_Return,
    Input_Tab,
    Input_Space,
    Input_Backspace,
    Input_Escape,
    Input_Tick,
    Input_Win,
    Input_Shift,
    Input_CapsLock,
    Input_Alt,
    Input_Control,
    Input_RightWin,
    Input_RightShift,
    Input_RightAlt,
    Input_RightControl,
    Input_Function,
    Input_F1,
    Input_F2,
    Input_F3,
    Input_F4,
    Input_F5,
    Input_F6,
    Input_F7,
    Input_F8,
    Input_F9,
    Input_F10,
    Input_F11,
    Input_F12,
    Input_F13,
    Input_F14,
    Input_F15,
    Input_F16,
    Input_F17,
    Input_F18,
    Input_F19,
    Input_F20,
    Input_F21,
    Input_F22,
    Input_F23,
    Input_F24,
    Input_Help,
    Input_Home,
    Input_Insert,
    Input_PageUp,
    Input_ForwardDelete,
    Input_End,
    Input_PageDown,
    Input_LeftArrow,
    Input_RightArrow,
    Input_DownArrow,
    Input_UpArrow,

    Input_KeyboardDiscreteCount,
};


enum InputMouseDiscrete : uint32 {
    Input_MouseLeft,
    Input_MouseRight,
    Input_MouseMiddle,

    Input_MouseDiscreteCount,
};

enum InputMouseAnalogue : uint32 {
    Input_MousePositionX,
    Input_MousePositionY,
    Input_MousePositionXNorm,
    Input_MousePositionYNorm,

    Input_MousePositionXOffset,
    Input_MousePositionYOffset,
        
    Input_ScrollDirection,

    Input_MouseAnalogueCount,
};


struct InputDevice {
    InputDeviceType type;

    int32 discreteCount;
    
    int32 *framesHeld;
    bool *released;
    bool *pressed;
    real32 *timePressed;

    int32 analogueCount;
    
    real32 *prevAnalogue;
    real32 *analogue;
};

struct InputEvent {
    InputDevice *device;
    int32 index;
    bool discreteValue;
    real32 value;
};


struct InputManager {
    uint32 capacity;
    uint32 count;
    uint32 size;

    DynamicArray<InputEvent> events;

    int32 deviceCount;
    InputDevice *devices;

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
