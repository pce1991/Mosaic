
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
    Mosaic->screenColor = RGB(0.4f, 0.4f, 0.4f);
}

// This is where you put the code you want to run every update.
// This function is called every frame, and its what tells us what colors to draw
// all the tiles at, along with all the other state changes in our game.

void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = RGB(0.0f, 0.0f, 0.0f);
        }
    }
}
