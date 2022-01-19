// This function runs at the start of our program
void MyMosaicInit() {
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);
    // SetTileColor takes 5 arguments,
    // the x and y position, and then the red, green, and blue colors
    SetTileColor(0, 0, 1, 0, 0);
    SetTileColor(1, 0, 0, 1, 0);
    SetTileColor(0, 1, 0, 0, 1);
}
