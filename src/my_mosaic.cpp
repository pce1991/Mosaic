

void MyMosaicInit() {
    SetMosaicGridSize(16, 16);

}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    // The position will always clamp to the bottom left corner of whatever tile
    // the position is inside.
    DrawTextTile(V2(0, 0), 0.3f, RGB(1, 1, 1), "Tile text");
    DrawTextTile(V2(0, 1), 0.3f, RGB(1, 1, 1), "Tile text");
    DrawTextTile(V2(1, 2), 0.3f, RGB(1, 1, 1), "Tile text");

    // @NOTE: we can draw text at a tile position "off" the screen,
    DrawTextTile(V2(16, 4), 0.3f, RGB(1, 1, 1), "Tile text");
    DrawTextTile(V2(17, 5), 0.3f, RGB(1, 1, 1), "Tile text");

    // works in normalized space where screen coordinates go from 0 to 1
    DrawTextScreen(V2(0.5f, 0.5f), 0.02f, RGB(1, 0, 0), "Screen text");
    
    SetTileColor(0, 0, 0.0f, 0.5f, 0.8f);
    SetTileColor(0, 1, 0.0f, 0.5f, 0.8f);
    SetTileColor(1, 2, 0.0f, 0.5f, 0.8f);
}

