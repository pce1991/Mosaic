
struct MyData {
    /*
      Put all your game specific data in here.
     */
};

MyData Data = {};

void MyMosaicInit() {
    
    SetMosaicGridSize(16, 16);

    ShowGrid();

    SetGridColor(0.5f, 0.5f, 0.5f);

    // These color values are "normalized" meaning that max is 1 and min is 0
    SetMosaicScreenColor(0.2f, 0.2f, 0.2f);

    // This writes a string to the log.txt file that gets written into the output directory
    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}

// This is where you put the code you want to run every update.
// This function is called every frame, and its what tells us what colors to draw
// all the tiles at, along with all the other state changes in our game.
void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);
    
    SetTileColor(5, 4, 0.5, 0.5, 0.5);

    vec4 color = RGB(0.1f, 0.8f, 0.8f);
    SetTileColor(6, 6, color);
    // You can also pass a vec4 to SetTileColor
}

