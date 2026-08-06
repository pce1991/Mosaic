

struct MyData {
    Sprite sprite;
    Sprite sprite2;
};

MyData Data = {};

void MyGameInit() {
    // All sprites must be png files, no jpegs or anything

    LoadSprite("data/textures/galaga_ship.png", &Data.sprite);

    LoadSprite("data/textures/van_gogh.png", &Data.sprite2);
}

void MyGameUpdate() {
    // version that doesnt take an angle.
    DrawInstancedSprite(&Core->graphics.spriteBuffer, 0, V2(0), V2(4, 4), &Data.sprite2);
    DrawInstancedSprite(&Core->graphics.spriteBuffer, 1, V2(0), V2(1, 1), Time, &Data.sprite);
}
