
void MyMosaicInit() {
    
}

int32 x = 0;

void MyMosaicUpdate() {
    SetTileColor(0, 0, 1, 0, 0);

    if (InputHeld(Game->keyboard, Input_Space)) {
        SetTileColor(0, 0, 1, 1, 0);
    }

    if (InputHeldSeconds(Game->keyboard, Input_Space, 1.0f)) {
        SetTileColor(0, 0, 1, 1, 1);
    }

    if (InputPressed(Game->keyboard, Input_RightArrow)) {
        x++;
    }

    if (InputPressed(Game->mouse, Input_MouseLeft)) {
        x++;
    }
    
    SetTileColor(x, 0, 1, 1, 0);
}
