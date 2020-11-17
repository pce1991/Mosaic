
void MyMosaicInit() {
    SetMosaicGridSize(9, 9);
}

void MyMosaicUpdate() {
    ClearColor(RGB(0, 0, 0));

    vec2i mousePos = GetMousePosition();
    SetTileColor(mousePos.x, mousePos.y, 1, 1, 1);
}
