
void MyMosaicInit() {
    SetMosaicGridSize(9, 9);
}

real32 r = 0.0f;
real32 b = 0.0f;

void MyMosaicUpdate() {

    r = Abs(sinf(Time));
    b = Abs(cosf(Time));
    
    if (InputPressed(Input, Input_MouseLeft)) {
        vec2i mousePos = GetMousePosition();
        SetTileColor(mousePos.x, mousePos.y, r, 0, b);
    }
}
