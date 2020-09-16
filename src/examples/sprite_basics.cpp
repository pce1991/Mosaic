

struct MyData {
    Sprite sprite;
};

MyData *Data = NULL;

void MyInit() {
    Game->myData = malloc(sizeof(MyData));
    memset(Game->myData, 0, sizeof(MyData));

    Data = (MyData *)Game->myData;

    LoadSprite(&Data->sprite, "data/galaga_ship.png");
    OpenGL_InitTexture(&Data->sprite);
}

void MyGameUpdate() {

    DrawSprite(V2(0), V2(1), &Data->sprite);
}
