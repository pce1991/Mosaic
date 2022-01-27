// This function runs at the start of our program
void MyMosaicInit() {
}

// Put variables in global scope if you want them to persist between frames.
real32 red = 0.0f;
real32 xPos = 0.0f;

real32 red1 = 0;
real32 green1 = 0;
real32 blue1 = 0;

// MyMosaicUpdate is called once per frame and does all the updating of the state
// that we want to happen before drawing an image to the screen.
void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);
    // SetTileColor takes 5 arguments,
    // the x and y position, and then the red, green, and blue colors
    SetTileColor(0, 0, 1, 0, 0);
    SetTileColor(1, 0, 0, 1, 0);
    SetTileColor(0, 1, 0, 0, 1);

    // Because red is declared in the scope of this function it is going
    // to get redeclared every frame which means we cant accumulate a value.
    // real32 red = 0.5f;
    // red += 0.1f;

    // Make this frame-rate indepedent by multiplying by DeltaTime.

    red += 0.1f * DeltaTime;
    SetTileColor(0, 2, red, 0, 0.0f);

    xPos += 4 * DeltaTime;
    SetTileColor(xPos, 3, 0.4f, 1.0f, 0.2f);

    if (red1 < 0.5) {
        red1 += 0.2f * DeltaTime;
    }
    if (red1 >= 0.5) {
        if (green1 < 0.3) {
            green1 += 0.5f * DeltaTime;
        }
    }
    if (green1 >= 0.3) {
        blue1 += 0.2f * DeltaTime;
        green1 -= 0.2f * DeltaTime;
    }
    if (blue1 >= 1) {
        red1 -= 0.1f * DeltaTime;
        green1 += 0.5f * DeltaTime;
    }
    
    // green1 += 0.1f * DeltaTime;
    
    
    SetTileColor(0, 4, red1, green1, blue1);
}
