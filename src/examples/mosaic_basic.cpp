
struct MyData {
    /*
      Put all your game specific data in here.
     */
};

MyData *Data = NULL;

void MyMosaicInit() {
    // Here we allocate the data defined in MyData so we have it on the first frame.
    Data = (MyData *)malloc(sizeof(MyData));
    memset(Data, 0, sizeof(MyData));
    // I like to clear all my data to 0 and then manually set anything I don't want to start at 0.

    /*
      You can call SetMosaicGridSize(n, m) to set the grid size
      Look at mosaic.h for the MosaicMem struct to see various parameters you can set
     */

    SetMosaicGridSize(16, 16);

    // We have a global pointer to the MosaicMem data so you can set things like this.
    Mosaic->drawGrid = true;

    Mosaic->gridColor = RGB(0.2f, 0.2f, 0.2f);

    // These color values are "normalized" meaning that max is 1 and min is 0
    Mosaic->screenColor = RGB(0.3f, 0.7f, 1.0f);

    // This writes a string to the log.txt file that gets written into the output directory
    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}



// Globals variables
int32 keyframeIndex = 0;
real32 lastTimeKeyframeChanged = 0.0f;

real32 blue = 0.0f;

vec2i guyPosition = V2i(12, 5);


// This is where you put the code you want to run every update.
// This function is called every frame, and its what tells us what colors to draw
// all the tiles at, along with all the other state changes in our game.
void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    real32 timeSinceKeyframeChanged = Game->time - lastTimeKeyframeChanged;

    
    // With the spacebar example the input was what we recorded
    // But here we use the time since to detect when its time to record
    // a change in the keyframe.
    if (timeSinceKeyframeChanged >= 1.0f) {
        keyframeIndex++;

        lastTimeKeyframeChanged = Game->time;

        if (keyframeIndex > 3) {
            keyframeIndex = 0;
        }
    }

    if (InputPressed(Input, Input_RightArrow)) {
        blue += 0.1f;
    }
    
    if (InputPressed(Input, Input_LeftArrow)) {
        blue -= 0.1f;
    }

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = RGB(0.0f, 0.0f, 0.0f);

            if (keyframeIndex == 0) {
                tile->color = RGB(0.8f, 0.5f, 0.0f + blue);
            }
            if (keyframeIndex == 1) {
                tile->color = RGB(1.0f, 0.0f, 0.0f + blue);
            }
            if (keyframeIndex == 2) {
                tile->color = RGB(1.0f, 0.6f, 0.6f + blue);
            }
            if (keyframeIndex == 3) {
                tile->color = RGB(0.0f, 0.6f, 0.6f + blue);
            }
        }
    }

    // If we do this with InputHeld we probably want to use a timer
    // so that we can enforce that it only moves 1 square per
    // n seconds
    if (InputPressed(Input, Input_Up)) {
        guyPosition.y -= 1;

        // Lets clamp onto the screen:
        if (guyPosition.y < 0) {
            guyPosition.y = 0;
        }
    }

    if (InputPressed(Input, Input_Down)) {
        guyPosition.y += 1;

        // Lets clamp onto the screen:
        // This variable stores the current value of gridHeight
        if (guyPosition.y > Mosaic->gridHeight - 1) {
            guyPosition.y = Mosaic->gridHeight - 1;
        }
    }

    

    // Get the tile at a certain position, and then set the color of that tile

    // guyTile is a "pointer" to the location of one of our tiles
    // If we ask for a tile that is outside our board, then
    // guyTile will be NULL
    Tile *guyTile = GetTile(guyPosition.x, guyPosition.y);
    // This "dot" operator "." this gives us the "member value"
    // of our vec2i value

    // This checks to make sure that guyTile is not NULL
    if (guyTile != NULL) {
        guyTile->color = RGB(1, 1, 1);
    }
}
