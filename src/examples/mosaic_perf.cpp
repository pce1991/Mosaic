
Sprite shipSprite = {};

void MyMosaicInit() {
    //SetMosaicGridSize(320, 240);
    SetMosaicGridSize(640, 480);
    
    LoadSprite(&shipSprite, "data/galaga_ship.png");
}

void MyMosaicUpdate() {
    
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            r32 r = x / (Mosaic->gridWidth * 1.0f);
            r32 b = y / (Mosaic->gridHeight * 1.0f);
            r32 g = (1 + sinf(Time)) * 0.5f;
            SetTileColor(x, y, r, g, b);
        }
    }

    for (int i = 0; i < 10; i++) {
        DrawSprite(V2(40 + (40 * sinf(Time + i)), 40 + (i * 20)), &shipSprite);
    }

    // @TODO: demonstrate how we can use multi-threading to improve performance
    //HANDLE threadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Proc, &platform.scheduler, 0, &threadID);
}

