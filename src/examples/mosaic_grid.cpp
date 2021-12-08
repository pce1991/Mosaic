// To compile the engine:
// open x64 native tools
// Navigate inside the Mosaic directory
// type "win_compile.bat" 

struct MyData {
    /*
      Put all your game specific data in here.
     */
};

MyData Data = {};

void MyMosaicInit() {
    SetMosaicGridSize(16, 16);
    HideGrid();

    SetGridColor(0.5f, 0.5f, 0.5f);

    // These color values are "normalized" meaning that max is 1 and min is 0
    SetMosaicScreenColor(0.2f, 0.2f, 0.2f);

    // This writes a string to the log.txt file that gets written into the output directory
    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    SetTileColor(0, 0, 1, 1, 1);

    if (InputPressed(Input, Input_Space)) {
        SetMosaicGridSize(RandiRange(8, 64), RandiRange(16, 64));
    }

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            SetTileColor(x, y, 0.0f, x / (Mosaic->gridWidth * 1.0f), y / (Mosaic->gridHeight * 1.0f));
        }
    }

    DrawTextScreenPixel(&Game->monoFont, V2(800, 35), 24.0f, V4(1), true, "w: %d, h: %d", Mosaic->gridWidth, Mosaic->gridHeight);
}
// To compile the engine:
// open x64 native tools
// Navigate inside the Mosaic directory
// type "win_compile.bat" 

struct MyData {
    /*
      Put all your game specific data in here.
     */
};

MyData Data = {};

void MyMosaicInit() {
    SetMosaicGridSize(16, 16);
    HideGrid();

    SetGridColor(0.5f, 0.5f, 0.5f);

    // These color values are "normalized" meaning that max is 1 and min is 0
    SetMosaicScreenColor(0.2f, 0.2f, 0.2f);

    // This writes a string to the log.txt file that gets written into the output directory
    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    SetTileColor(0, 0, 1, 1, 1);

    if (InputPressed(Input, Input_Space)) {
        SetMosaicGridSize(RandiRange(8, 64), RandiRange(16, 64));
    }

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            SetTileColor(x, y, 0.0f, x / (Mosaic->gridWidth * 1.0f), y / (Mosaic->gridHeight * 1.0f));
        }
    }

    DrawTextScreenPixel(&Game->monoFont, V2(800, 35), 24.0f, V4(1), true, "w: %d, h: %d", Mosaic->gridWidth, Mosaic->gridHeight);
}
