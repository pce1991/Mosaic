
void MyMosaicInit() {
    SetMosaicGridSize(16, 16);
}


real32 x = 0;
real32 y = 2;

real32 red = 0;
real32 green = 1;
real32 blue = 0;

void Simple() {
    SetAllTileColors(0, 0, 0);

    if (x < 8) {
        x += Game->deltaTime * 3;
    }

    if (x >= 8) {
        y += Game->deltaTime * 8;
    }

    if (y > 2) {
        blue += 1.0f * Game->deltaTime;
    }

    if (y > 15) {
        y = 15;
        red += 1.0f * Game->deltaTime;
        green -= 1.0f * Game->deltaTime;
    }

    SetTileColor(x, y, red, green, blue);
}


// The attempt here is to avoid using &&s while still getting some more complex behavior
// I don't think it's particularly clear what's happening. I like the note about code order
// being disctinct from execution order, but it is a little forced here because we could
// make the execution and code order match if we were using &&
void Simple2() {
    SetAllTileColors(0, 0, 0);

    if (x < 5) {
        x += Game->deltaTime * 3;
    }

    // Notice that the order of our conditions is not necessarily the order
    // of events that we want to happen in our code. 
    if (x >= 10) {
        y -= Game->deltaTime * 4;
    }
    else if (x >= 5) {
        y += Game->deltaTime * 8;
    }

    if (y > 2) {
        blue += 1.0f * Game->deltaTime;
    }

    if (y > 6) {
        x += Game->deltaTime * 4;
    }

    SetTileColor(x, y, red, green, blue);
}

// There is a good opportunity here to introduce enums

void MyMosaicUpdate() {
    Simple();
}
