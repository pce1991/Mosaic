
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


real32 timeSpacePressed = 0.0f;

int32 keyframeIndex = 0;

real32 red = 0.0f;

// This is where you put the code you want to run every update.
// This function is called every frame, and its what tells us what colors to draw
// all the tiles at, along with all the other state changes in our game.
void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    // We want the keyframe index to change every n seconds
    // Which means we need a way to record how long its been since the last keyframe
    // (which means we need to know the time we started the last keyframe)
    // Then if its been long enough we goto the next keyframe.
    // Look at the example of the spacebar, instead of based on when the spacebar
    // see if you can find a way to base it on how many seconds since the keyframeIndex
    // has changed.

    if (InputPressed(Input, Input_Space)) {
        keyframeIndex++;
    }

    if (InputPressed(Input, Input_RightArrow)) {
        red = red + 0.1;
    }
    if (InputPressed(Input, Input_LeftArrow)) {
        red = red - 0.1;
    }

    // We have two things going on: we have a animation that changes based on time
    // and then we augment that animation based on our input.

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = RGB(0.0f, 0.0f, 0.0f);

            if (keyframeIndex == 0) {
                tile->color = RGB(1.0f + red, 0.0f, 0.0f);
            }
            if (keyframeIndex == 1) {
                tile->color = RGB(0.0f + red, 1.0f, 0.0f);
            }
        }
    }

    // To make a variable we give it the type
    // This tells us how big the data and how to interpret it.
    real32 t = 1.0f;

    if (InputPressed(Input, Input_Space)) {
        timeSpacePressed = Game->time;
    }
    

    // The "arrow" operator -> is a way to access members of a pointer
    real32 timeSinceKeyframe = Game->time - timeKeyframeChanged;

    if (Mosaic->hoveredTile) {
        Mosaic->hoveredTile->color = V4(1, 1, 1, 1);

        if (timeSincePressedSpace >= 5) {
            Mosaic->hoveredTile->color = V4(1, 0, 0, 1);
        }
    }
}
