


// @TODO: do these both in screenspace! it'll be way easier to do calculations

bool InputString(vec2 position, vec2 dimensions, real32 textSize, bool *active, bool *pressedEnter, char *str) {
    bool modified = false;
    vec2 rectPos = position;
    DrawRectScreen(rectPos, dimensions, V4(0.5f));
    //DrawTextScreen(position, textSize, V4(1, 0, 0, 1), str);

    if (InputPressed(Game->keyboard, Input_MouseLeft)) {
        *active = true;
    }

    int32 len = strlen(str);
    if (*active) {
        if (Input->charCount > 0) {
            str[len] = Input->inputChars[0];
            modified = true;
        }
    }

    if (InputPressed(Game->keyboard, Input_Return)) {
        *pressedEnter = true;
    }

    return modified;
}


bool Button(vec2 position, vec2 dimensions, vec4 color, real32 textSize, vec4 textColor, char *name) {
    bool modified = false;
    vec2 rectPos = position;

    Rect rect = {};
    rect.min = position;
    rect.max = position + V2(dimensions.x, dimensions.y);

    if (PointRectTest(rect, V2(Input->mousePos))) {
        if (InputPressed(Game->mouse, Input_MouseLeft)) {
            return true;
        }

        color = V4(1, 0, 0, 1);
    }

    //DrawRectScreen(rectPos, dimensions, color);
    //DrawText(V2(0), 8, textColor, name);
    //DrawTextScreen(V2(450) * sinf(Game->time), 1000 * sinf(Game->time), textColor, name);
    //DrawTextScreen(position, textSize, textColor, name);

    return false;
}

