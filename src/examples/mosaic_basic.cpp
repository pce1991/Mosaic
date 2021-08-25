
void MyMosaicInit() {
}

void MyMosaicUpdate() {
    // ClearTiles takes 3 arguments (red, green, blue) which are each a real number value
    // between 0 and 1
    ClearTiles(0, 0, 0);

    // (x, y, r, g, b)
    SetTileColor(0, 0, 1.0f, 0.0f, 0.0f);
    SetTileColor(1, 0, 0.0f, 1.0f, 0.0f);
    SetTileColor(0, 1, 0.0f, 0.0f, 1.0f);
}
