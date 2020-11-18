
void MyMosaicInit() {
    SetMosaicGridSize(16, 16);
}


real32 x = 0;
real32 y = 2;

real32 red = 0;
real32 green = 1;
real32 blue = 0;

void MyMosaicUpdate() {
    SetAllTileColors(0, 0, 0);

    if (x < 5 && y < 3) {
        x += Game->deltaTime * 3;
    }

    if (x >= 5 && y < 8) {
        y += Game->deltaTime * 6;
    }

    if (x >= 5 && y >= 8 && blue < 1) {
        blue += Game->deltaTime * 1;
    }

    if (x >= 5 && y >= 8 && blue >= 1) {
        x += Game->deltaTime * 8;
    }

    SetTileColor(x, y, red, green, blue);
}
