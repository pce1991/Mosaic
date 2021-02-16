
void MyMosaicInit() {
    SetMosaicGridSize(32, 32);
    HideGrid();

    SetGridColor(0.5f, 0.5f, 0.5f);

    SetMosaicScreenColor(0.2f, 0.2f, 0.2f);

    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}


void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

}
