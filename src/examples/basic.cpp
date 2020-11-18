
struct Basic {
    vec2 positions;
};

Basic *basic = NULL;

void MyInit() {
    Game->myData = malloc(sizeof(Basic));
    memset(Game->myData, 0, sizeof(Basic));

    basic = (Basic *)Game->myData;
}

void MyGameUpdate() {
    
    DrawRectScreen(V2(400 + 200 * sinf(Game->time), 400), V2(32.0f + sinf(Game->time), 32.0f), V4(0.5f, 0.5f, 0.5f, 0.5f));

    Log("deltaTime %f and x pos %f", Game->deltaTime, sinf(Game->time));
}
