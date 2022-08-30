

static int32 WinVirtualKeyMap[] = {
    -1,
    -1, // VK_LBUTTON
    -1, // VK_RBUTTON
    -1, // VK_CANCEL
    -1, // VK_MBUTTON
    -1, // VK_XBUTTON1
    -1, // VK_XBUTTON2

    -1, // Undefined
    Input_Backspace, // VK_BACK
    Input_Tab, // VK_TAB

    // Reserved
    -1, -1,
    -1, // VK_CLEAR

    Input_Return, // VK_RETURN

    // Undefined
    -1, -1, 

    Input_Shift, // VK_SHIFT
    Input_Control, // VK_CONTROL
    Input_Alt, // VK_MENU

    -1, // VK_PAUSE
    -1, // VK_CAPITAL
    -1, // VK_KANA, VK_HANGUEL VK_HANGUL
    -1, // Undefined
    -1, // VK_JUNJA
    -1, // VK_FINAL
    -1, // VK_HANJA, VK_KANJI

    -1, // Undefined
    Input_Escape, // VK_ESCAPE

    -1, // VK_CONVERT
    -1, //VK_NONCONVERT
    -1, //VK_ACCEPT
    -1, //VK_MODECHANGE

    Input_Space, // VK_SPACE
    -1, // VK_PRIOR
    -1, // VK_NEXT
    Input_End, // VK_END
    Input_Home, // VK_HOME
    Input_LeftArrow, // VK_LEFT
    Input_UpArrow, // VK_UP
    Input_RightArrow, // VK_RIGHT
    Input_DownArrow, // VK_DOWN
    -1, // VK_SELECT
    -1, // VK_PRINT
    -1, // VK_EXECUTE
    -1, // VK_SNAPSHOT
    Input_Insert, // VK_INSERT
    Input_ForwardDelete, // VK_DELETE
    Input_Help, // VK_HELP
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

    // Undefined
    -1, -1, -1, -1, -1, -1, -1,

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
    Input_Win, // VK_LWIN
    Input_RightWin, // VK_RWIN
    -1, // VK_APPS
    -1, // Reserved
    -1, // VK_SLEEP
    -1, // VK_NUMPAD0
    -1, // VK_NUMPAD1
    -1, // VK_NUMPAD2
    -1, // VK_NUMPAD3
    -1, // VK_NUMPAD4
    -1, // VK_NUMPAD5
    -1, // VK_NUMPAD6
    -1, // VK_NUMPAD7
    -1, // VK_NUMPAD8
    -1, // VK_NUMPAD9
    -1, // VK_MULTIPLY
    -1, // VK_ADD
    -1, // VK_SEPARATOR
    -1, // VK_SUBTRACT
    -1, // VK_DECIMAL
    -1, // VK_DIVIDE
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

    // Unassigned
    -1, -1, -1, -1, -1, -1, -1, -1,

    -1, // VK_NUMLOCK
    -1, // VK_SCROLL

    // OEM specific
    -1, -1, -1, -1, -1,

    // Unassigned
    -1, -1, -1, -1, -1, -1, -1, -1, -1,

    Input_Shift, // VK_LSHIFT
    Input_RightShift, // VK_RSHIFT
    Input_Control, // VK_LCONTROL
    Input_RightControl, // VK_RCONTROL

    Input_Alt, // VK_LMENU
    Input_RightAlt, // VK_RMENU
    
    -1, // VK_BROWSER_BACK
    -1, // VK_BROWSER_FORWARD
    -1, // VK_BROWSER_REFRESH
    -1, // VK_BROWSER_STOP
    -1, // VK_BROWSER_SEARCH
    -1, // VK_BROWSER_FAVORITES
    -1, // VK_BROWSER_HOME
    -1, // VK_VOLUME_MUTE
    -1, // VK_VOLUME_DOWN
    -1, // VK_VOLUME_UP
    -1, // VK_MEDIA_NEXT_TRACK
    -1, // VK_MEDIA_PREV_TRACK
    -1, // VK_MEDIA_STOP
    -1, // VK_MEDIA_PLAY_PAUSE
    -1, // VK_LAUNCH_MAIL
    -1, // VK_LAUNCH_MEDIA_SELECT
    -1, // VK_LAUNCH_APP1
    -1, // VK_LAUNCH_APP2

    // Reserved
    -1, -1,

    Input_Semicolon, // VK_OEM_1
    Input_Equal, // VK_OEM_PLUS
    Input_Comma, // VK_OEM_COMMA
    Input_Minus, // VK_OEM_MINUS
    Input_Period, // VK_OEM_PERIOD
    Input_Slash, // VK_OEM_2
    Input_Tick, // VK_OEM_3

    // Reserved
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1,
    
    // Unassigned
    -1, -1, -1,

    Input_LeftBracket, // VK_OEM_4
    Input_Backslash, // VK_OEM_5
    Input_RightBracket, // VK_OEM_6
    Input_Quote, // VK_OEM_7

    // VK_OEM_8...
};


// [0]: https://stackoverflow.com/questions/15966642/how-do-you-tell-lshift-apart-from-rshift-in-wm-keydown-events
WPARAM MapLeftRightKeys(WPARAM vk, LPARAM lParam) {
    WPARAM newVK = vk;
    UINT scancode = (lParam & 0x00ff0000) >> 16;
    int extended  = (lParam & 0x01000000) != 0;

    switch (vk) {
        case VK_SHIFT: {
            newVK = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
        } break;
        case VK_CONTROL: {
            newVK = extended ? VK_RCONTROL : VK_LCONTROL;
        } break;
        case VK_MENU: {
            newVK = extended ? VK_RMENU : VK_LMENU;
        } break;
        default: {
            // Not a key we map from generic to left/right specialized, sojust return it.
            newVK = vk;
        } break;
    }

    return newVK;
}


void WindowsGetInput(InputManager *input) {

    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

        switch (msg.message) {
            case WM_KEYDOWN: {
                TranslateMessage(&msg);
                
                int keycode = msg.wParam;

                InputEvent event = {};
                event.device = Game->keyboard;
                event.index = WinVirtualKeyMap[MapLeftRightKeys(msg.wParam, msg.lParam)];
                event.discreteValue = true;

                PushBack(&input->events, event);
            } break;

            case WM_KEYUP: {
                int keycode = msg.wParam;

                InputEvent event = {};
                event.device = Game->keyboard;
                event.index = WinVirtualKeyMap[MapLeftRightKeys(msg.wParam, msg.lParam)];
                event.discreteValue = false;

                PushBack(&input->events, event);
             } break;

            case WM_CHAR: {
                uint16 character = (uint16)msg.wParam;
                input->inputChars[input->charCount++] = character;
            } break;

            case WM_MOUSEMOVE : {
                int posX = GET_X_LPARAM(msg.lParam);
                int posY = GET_Y_LPARAM(msg.lParam);

                input->mousePos.x = posX;
                input->mousePos.y = Game->screenHeight - posY;
            } break;

                    
            case WM_LBUTTONDOWN : {
                InputEvent event = {};
                event.device = Game->mouse;
                event.index = Input_MouseLeft;
                event.discreteValue = true;

                PushBack(&input->events, event);
            } break;

            case WM_LBUTTONUP : {
                InputEvent event = {};
                event.device = Game->mouse;
                event.index = Input_MouseLeft;
                event.discreteValue = false;

                PushBack(&input->events, event);
            } break;
                    
            case WM_RBUTTONDOWN : {
                InputEvent event = {};
                event.device = Game->mouse;
                event.index = Input_MouseRight;
                event.discreteValue = true;

                PushBack(&input->events, event);
            } break;

            case WM_RBUTTONUP : {
                InputEvent event = {};
                event.device = Game->mouse;
                event.index = Input_MouseRight;
                event.discreteValue = false;

                PushBack(&input->events, event);
            } break;

            default : {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
}
