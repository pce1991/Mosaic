
void MyMosaicInit() {
    SetMosaicGridSize(16, 16);
}

vec2 position = V2(8, 8);
vec2 velocity = V2(0, 0);



void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    velocity.x = 0;

    // Problem with how we're treating real numbers as tile positions is stopping at (1, 1) means you'll go to
    // the left immediately but it'll take more time to get to the right.
    // We could either move instantly when we press, or we could treat 0.5 as the "rest" position when we
    // finish moving
    
    if (InputHeld(Input, Input_Left)) {
        velocity.x = -4;
    }
    if (InputHeld(Input, Input_Right)) {
        velocity.x = 4;
    }

    position = position + velocity * DeltaTime;

    SetTileColor(position, 1, 0, 0);
}
