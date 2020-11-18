
struct MyData {
    /*
      Put all your game specific data in here.
     */
};

MyData *Data = NULL;

void MyMosaicInit() {
    Data = (MyData *)malloc(sizeof(MyData));
    memset(Data, 0, sizeof(MyData));

    SetMosaicGridSize(16, 16);

    // We have a global pointer to the MosaicMem data so you can set things like this.
    Mosaic->drawGrid = false;

    Mosaic->gridColor = RGB(0.2f, 0.2f, 0.2f);

    // These color values are "normalized" meaning that max is 1 and min is 0
    //Mosaic->screenColor = RGB(0.3f, 0.7f, 1.0f);
    Mosaic->screenColor = RGB(0.2f, 0.2f, 0.2f);
}

DynamicArray<vec2> positions = {};

void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = RGB(0.0f, 0.0f, 0.0f);
        }
    }

    
    if (InputPressed(Input, Input_Up)) {
        vec2 p = V2(RandfRange(0.0f, 15.0f), RandfRange(0.0f, 15.0f));
        PushBack(&positions, p);
    }

    if (InputPressed(Input, Input_Down)) {
        if (positions.count > 0) {
            // We could remove an index we want, but here I want to delete the position
            // that's been in the longest.
            // You need to make sure when using RemoveAtIndex that the index you're
            // removing is less than the number of elements.
            RemoveAtIndex(&positions, 0);
        }
    }

    for (int i = 0; i < positions.count; i++) {
        vec2 p = positions[i];
        Tile *t = GetTile(p.x, p.y);

        t->color = RGB(1, 0, 0);
    }
}
