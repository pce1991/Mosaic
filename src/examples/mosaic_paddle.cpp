
void MyMosaicInit() {
    SetMosaicGridSize(90, 60);
    HideGrid();

    SetGridColor(0.5f, 0.5f, 0.5f);

    SetMosaicScreenColor(0.2f, 0.2f, 0.2f);

    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}

vec2 pos = V2(16, 26);
vec2 vel = V2(0, 0);

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    int32 mouseX = GetMousePositionX();
    

    int32 dir = 0;
    if (mouseX > pos.x) {
        dir = 1;
    }
    else if (mouseX < pos.x) {
        dir = -1;
    }

    real32 speed = 200;
    vel.x = dir * speed;

    real32 newPosX = pos.x + vel.x * DeltaTime;

    if (dir == 1 && newPosX > mouseX) {
        newPosX = mouseX;
    }
    else if (dir == -1 && newPosX < mouseX) {
        newPosX = mouseX;
    }

    pos.x = newPosX;

    for (int i = 0; i < 6; i++) {
        SetTileColor(pos.x + i, pos.y, 0, 1, 1);
    }

    DrawTextScreenPixel(&Game->monoFont, V2(800, 35), 24.0f, V4(1), "%d", mouseX);
    DrawTextScreenPixel(&Game->monoFont, V2(800, 70), 24.0f, V4(1), "%f", pos.x);
}
