
// @NOTE: this file is included from linux.cpp AFTER game.cpp so that the input
// types (InputManager, InputEvent, Core, etc) are all available.

static int32 KeySymToInput(KeySym keySym) {
    // letters (base keysym is the lowercase letter for most layouts)
    if (keySym >= XK_a && keySym <= XK_z) {
        return Input_A + (keySym - XK_a);
    }
    if (keySym >= XK_A && keySym <= XK_Z) {
        return Input_A + (keySym - XK_A);
    }

    // number row
    if (keySym >= XK_0 && keySym <= XK_9) {
        return Input_0 + (keySym - XK_0);
    }

    switch (keySym) {
        case XK_BackSpace: return Input_Backspace;
        case XK_Tab: return Input_Tab;
        case XK_Return: return Input_Return;
        case XK_Escape: return Input_Escape;
        case XK_space: return Input_Space;

        case XK_Home: return Input_Home;
        case XK_End: return Input_End;
        case XK_Page_Up: return Input_PageUp;
        case XK_Page_Down: return Input_PageDown;
        case XK_Insert: return Input_Insert;
        case XK_Delete: return Input_ForwardDelete;

        case XK_Left: return Input_LeftArrow;
        case XK_Right: return Input_RightArrow;
        case XK_Up: return Input_UpArrow;
        case XK_Down: return Input_DownArrow;

        case XK_Shift_L: return Input_Shift;
        case XK_Shift_R: return Input_RightShift;
        case XK_Control_L: return Input_Control;
        case XK_Control_R: return Input_RightControl;
        case XK_Alt_L: return Input_Alt;
        case XK_Alt_R: return Input_RightAlt;
        case XK_Caps_Lock: return Input_CapsLock;
        case XK_Super_L: return Input_Win;
        case XK_Super_R: return Input_RightWin;

        case XK_semicolon: return Input_Semicolon;
        case XK_equal: return Input_Equal;
        case XK_comma: return Input_Comma;
        case XK_minus: return Input_Minus;
        case XK_period: return Input_Period;
        case XK_slash: return Input_Slash;
        case XK_grave: return Input_Tick;
        case XK_bracketleft: return Input_LeftBracket;
        case XK_bracketright: return Input_RightBracket;
        case XK_backslash: return Input_Backslash;
        case XK_apostrophe: return Input_Quote;
    }

    // function keys
    if (keySym >= XK_F1 && keySym <= XK_F24) {
        return Input_F1 + (keySym - XK_F1);
    }

    return -1;
}


void LinuxGetInput(InputManager *input) {
    XEvent event;

    while (XPending(Platform->display) > 0) {
        XNextEvent(Platform->display, &event);

        switch (event.type) {
            case KeyPress: {
                KeySym keySym = XLookupKeysym(&event.xkey, 0);
                int32 index = KeySymToInput(keySym);

                if (index >= 0) {
                    InputEvent inputEvent = {};
                    inputEvent.device = Core->keyboard;
                    inputEvent.index = index;
                    inputEvent.discreteValue = true;

                    PushBack(&input->events, inputEvent);
                }

                // character input (the equivalent of WM_CHAR on windows)
                char chars[8];
                int charCount = XLookupString(&event.xkey, chars, sizeof(chars), NULL, NULL);
                for (int i = 0; i < charCount; i++) {
                    if (input->charCount < input->charSize) {
                        input->inputChars[input->charCount++] = chars[i];
                    }
                }
            } break;

            case KeyRelease: {
                // @NOTE: when a key auto-repeats, X sends a fake KeyRelease right before the
                // repeated KeyPress, so drop a release that is immediately followed by a press
                // of the same key at the same time.
                if (XPending(Platform->display) > 0) {
                    XEvent nextEvent;
                    XPeekEvent(Platform->display, &nextEvent);
                    if (nextEvent.type == KeyPress &&
                        nextEvent.xkey.keycode == event.xkey.keycode &&
                        nextEvent.xkey.time == event.xkey.time) {
                        continue;
                    }
                }

                KeySym keySym = XLookupKeysym(&event.xkey, 0);
                int32 index = KeySymToInput(keySym);

                if (index >= 0) {
                    InputEvent inputEvent = {};
                    inputEvent.device = Core->keyboard;
                    inputEvent.index = index;
                    inputEvent.discreteValue = false;

                    PushBack(&input->events, inputEvent);
                }
            } break;

            case MotionNotify: {
                int32 posX = event.xmotion.x;
                int32 posY = event.xmotion.y;

                // @NOTE: convert window client coords back into internal render
                // resolution coords (the frame may be letterboxed and scaled).
                real32 scale = Core->graphics.presentScale.x;
                vec2 offset = Core->graphics.presentOffset;

                int32 ix = (int32)((posX - offset.x) / scale);
                int32 iy = (int32)((posY - offset.y) / scale);

                int32 maxX = (int32)Core->graphics.resolutionWidth - 1;
                int32 maxY = (int32)Core->graphics.resolutionHeight - 1;
                if (ix < 0) ix = 0;
                if (iy < 0) iy = 0;
                if (ix > maxX) ix = maxX;
                if (iy > maxY) iy = maxY;

                input->mousePos.x = ix;
                input->mousePos.y = Core->graphics.resolutionHeight - iy;
            } break;

            case ButtonPress: {
                int32 index = -1;
                if (event.xbutton.button == 1) { index = Input_MouseLeft; }
                else if (event.xbutton.button == 2) { index = Input_MouseMiddle; }
                else if (event.xbutton.button == 3) { index = Input_MouseRight; }

                if (index >= 0) {
                    InputEvent inputEvent = {};
                    inputEvent.device = Core->mouse;
                    inputEvent.index = index;
                    inputEvent.discreteValue = true;

                    PushBack(&input->events, inputEvent);
                }
            } break;

            case ButtonRelease: {
                int32 index = -1;
                if (event.xbutton.button == 1) { index = Input_MouseLeft; }
                else if (event.xbutton.button == 2) { index = Input_MouseMiddle; }
                else if (event.xbutton.button == 3) { index = Input_MouseRight; }

                if (index >= 0) {
                    InputEvent inputEvent = {};
                    inputEvent.device = Core->mouse;
                    inputEvent.index = index;
                    inputEvent.discreteValue = false;

                    PushBack(&input->events, inputEvent);
                }
            } break;

            case ClientMessage: {
                if ((Atom)event.xclient.data.l[0] == Platform->wmDeleteWindow) {
                    PlatformRunning = false;
                }
            } break;

            case ConfigureNotify: {
                int32 clientWidth = event.xconfigure.width;
                int32 clientHeight = event.xconfigure.height;

                if (clientWidth > 0 && clientHeight > 0) {
                    SetWindowSize(clientWidth, clientHeight);
                    Platform->presentScale = Core->graphics.presentScale.x;
                    Platform->presentOffsetX = Core->graphics.presentOffset.x;
                    Platform->presentOffsetY = Core->graphics.presentOffset.y;
                }
            } break;
        }
    }
}
