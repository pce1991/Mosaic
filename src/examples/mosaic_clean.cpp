
struct MyData {
};

MyData *Data = NULL;

void MyMosaicInit() {
    Data = (MyData *)malloc(sizeof(MyData));
    memset(Data, 0, sizeof(MyData));
}

// This is where you put the code you want to run every update.
void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = V4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}
