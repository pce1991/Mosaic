
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



vec2 guyPosition = V2(12.0f, 5.0f);


// This is where you put the code you want to run every update.
// This function is called every frame, and its what tells us what colors to draw
// all the tiles at, along with all the other state changes in our game.
void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;


    // This uses time and a duration to calculate a percentage.
    real32 t = Game->time / 10.0f;

    // Here is an example of calculating t using the position,
    // and subtracting it from 1 so that 0% is at position 16,
    // and 100% is at position 0
    //real32 t = 1 - (guyPosition.y / 16.0f);
    
    // Lerp stands for "linear interpolation"
    real32 red = Lerp(0.0f, 1.0f, t);

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = RGB(red, 0.0f, 0.0f);
        }
    }

    
    
    if (InputPressed(Input, Input_Down)) {
        guyPosition.y += 1.0f;
    }
        
    if (InputPressed(Input, Input_Up)) {
        guyPosition.y -= 1.0f;
    }


    {
        real32 t = Game->time / 15.0f;
        // Its very important that we keep our t values between 0 and 1
        // So in this case since time will presumably get > 15 eventually
        // what we want to do is put a "clamp" on it and say
        if (t > 1) {
            t = 1;
        }

        real32 x = Lerp(0.0f, 15.0f, t);

        // One thing to note about GetTile is it expects integers right?
        // So what happens if we give it a number like 8.1583?
        // What it does it is "casts" that real number to an integer
        // which means it chops off the decimal part and its just 8.
        Tile *otherTile = GetTile(x, 7);

        if (otherTile != NULL) {
            otherTile->color = RGB(0.8f, 0.5f, 1.0f);
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

    if (InputHeldSeconds(Input, Input_MouseLeft, 2.0f)) {
        guyTile->color = RGB(1, 0, 0);
    }
}
