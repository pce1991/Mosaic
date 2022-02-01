
// This function runs at the start of our program
void MyMosaicInit() {
    SetMosaicGridSize(16, 16);
}

// MyMosaicUpdate is called once per frame and does all the updating of the state
// that we want to happen before drawing an image to the screen.
void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);
}
