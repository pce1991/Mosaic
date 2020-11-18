

struct MyData {
    Sprite sprite;

    Sprite sprite2;
};

MyData *Data = NULL;

void MyInit() {
    Game->myData = malloc(sizeof(MyData));
    memset(Game->myData, 0, sizeof(MyData));

    Data = (MyData *)Game->myData;

    // All sprites must be png files, no jpegs or anything

    LoadSprite(&Data->sprite, "data/galaga_ship.png");
    OpenGL_InitTexture(&Data->sprite);

    LoadSprite(&Data->sprite2, "data/van_gogh.png");
    OpenGL_InitTexture(&Data->sprite2);
}

void MyGameUpdate() {
    // version that doesnt take an angle.
    DrawSprite(V2(0), V2(4, 4), &Data->sprite2);

    DrawSprite(V2(0), V2(1, 1), Game->time, &Data->sprite);
}
