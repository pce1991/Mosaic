
real32 Timer = 0.0f;

void MyMosaicInit() {
    SetMosaicGridSize(19, 19);
    Mosaic->drawGrid = true;

    Mosaic->screenColor = V4(0.0f, 0.0f, 0.0f, 1.0f);
    Mosaic->boardColor = V4(0, 0, 0, 1.0f);
    Mosaic->lineColor = V4(0.4f, 0.4f, 0.4f, 1.0f);
}

// This is where you put the code you want to run every update.
void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    Timer += Game->deltaTime;

    if (Timer > 1.0f) {
        for (int y = 0; y < Mosaic->gridHeight; y++) {
            for (int x = 0; x < Mosaic->gridWidth; x++) {
                Tile *tile = GetTile(x, y);
                tile->color = V4(RandfRange(0.0f, 1.0f), 0.0f, 0.0f, 1.0f);
            }
        }

        Timer = 0.0f;
    }
}
