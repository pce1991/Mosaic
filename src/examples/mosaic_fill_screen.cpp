

void MyMosaicInit() {
    SetMosaicGridSize(16, 16);
}

// This is where you put the code you want to run every update.

real32 x = 0;
real32 y = 0;

real32 r = 0;
real32 g = 0.6f;
real32 b = 0.4f;

void MyMosaicUpdate() {
    // This function MyMosaicUpdate() is called once every
    // frame, which means any variables we create in it
    // only exist inside the scope of the function.
    // So I want to create these variables outside the scope
    // of my function so their data doesnt get reset everytime
    // we enter the function.

    // MyMosaicUpdate represents everything that happens
    // during a frame.

    SetTileColor(x, y, r, g, b);
    
    x += 0.5f;

    r += 0.025f;
    g -= 0.05f;
    b += 0.0125f;
    
    // 0.1f per frame, 60 frames per second, how many frames
    // for x to equal 1? 10 frames!

    if (x >= 16) {
        y += 1;
        x = 0;

        r = y * 0.05f;
        g = 0.6f + (y * 0.05f);
        b = 0.4f - (y * 0.05f);
    }
}
