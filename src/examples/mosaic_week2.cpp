
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
    Mosaic->drawGrid = false;

    Mosaic->gridColor = RGB(0.2f, 0.2f, 0.2f);

    // These color values are "normalized" meaning that max is 1 and min is 0
    Mosaic->screenColor = RGB(0.3f, 0.7f, 1.0f);

    // This writes a string to the log.txt file that gets written into the output directory
    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}

// This is what's known as a "global variable", meaning it is one level above the scope
// of all fuctions.
int32 frameIndex = 0;

// This is where you put the code you want to run every update.
// This function is called every frame, and its what tells us what colors to draw
// all the tiles at, along with all the other state changes in our game.
void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    // This variable is declared "in the scope" of MyMosaicUpdate
    // which means it gets created every frame.
    // int32 frameIndex = 0;

    if (InputPressed(Input, Input_Space)) {
        // This is setting the value of frameIndex to 1 greater than it already is
        // We call this an "increment"
        frameIndex += 1;

        // if (frameIndex == 3) {
        //     frameIndex -= 3;
        // }

        // if (frameIndex > 2) {
        //     frameIndex -= 3;
        // }

        // These are all equivalent ways to do the same thing,
        // but I like this one because it's robust, meaning that it works for
        // any value of frameIndex > 2, and we always go back to the beginning
        // no matter what the value of frameIndex is.
        if (frameIndex > 2) {
            frameIndex = 0;
        }

        // This "prints" the value of frameIndex into a file that we generate called
        // log.txt in the output directory.
        Log("frameIndex %d", frameIndex);
    }
    
    // 3 frame animation:
    // 3 seconds
    // RGB(0.8f, 0.8f, 0.0f);
    // RGB(1.0f, 0.6f, 0.0f);
    // RGB(0.7f, 0.0f, 0.6f);
    
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);

            // Conditional 
            if (frameIndex == 0) {
                tile->color = RGB(0.8f, 0.8f, 0.0f);
            }
            if (frameIndex == 1) {
                tile->color = RGB(1.0f, 0.6f, 0.0f);
            }
            if (frameIndex == 2) {
                tile->color = RGB(0.7f, 0.0f, 0.6f);
            }

            // This is a "fallback" if for some reason reach the end of our animation and dont loop.
            if (frameIndex > 2) {
                tile->color = RGB(0, 0, 0);
            }
        }
    }

    if (Mosaic->hoveredTile) {
        Mosaic->hoveredTile->color = V4(1, 1, 1, 1);
    }
}
