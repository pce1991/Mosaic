
void MyMosaicInit() {
    
}

float32 red = 0.0f;
int32 xPos = 0;

bool toggleBlue = true;

void MyMosaicUpdate() {

    // This is an example of a function that returns a value
    // which means we can store that value in a variable.
    bool pressed = InputPressed(Keyboard, Input_Space);
    if (pressed) {
        red = red + 0.1f;
    }

    if (InputPressed(Keyboard, Input_Tab)) {
        // if (toggleBlue) {
        //     toggleBlue = false;
        // }
        // else if (!toggleBlue) {
        //     toggleBlue = true;
        // }

        // if (toggleBlue) {
        //     toggleBlue = false;
        // }
        // else {
        //     toggleBlue = true;
        // }

        toggleBlue = !toggleBlue;
    }
    
    if (InputPressed(Keyboard, Input_RightArrow)) {
        xPos = xPos + 1;
    }

    if (xPos > 15) {
        //xPos = 15;
        xPos = 0;
    }

    // What if we wanted to make sure that xPos never leaves our grid
    
    if (toggleBlue) {
        SetTileColor(xPos, 0, red, 0.2f, 0.8f);
    }
    else {
        SetTileColor(xPos, 0, red, 0.2f, 0.2f);
    }
}
