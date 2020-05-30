
struct MyData_ {
    bool *cells;
};

MyData_ data = {};

void MyInit() {
    ComputeGridSize(64, 64);

    data.cells = (bool *)malloc(sizeof(bool) * Mosaic->tileCapacity);

    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        data.cells[i] = RandfRange(0.0f, 1.0f) < 0.3f;
    }
}

#if 0
void MosaicUpdate() {
    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        Tile *tile = &Tiles[i];
        
        if (data.cells[i]) {
            tile->color = V4(1);
        }
        else {
            tile->color = V4(0);
        }
    }

    if (Mosaic->hoveredTile != NULL) {
        Mosaic->hoveredTile->color = V4(1, 1, 1, 1);
    }
}
#endif
