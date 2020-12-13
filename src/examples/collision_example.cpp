
struct MyData {
    vec2 aPos;
    Rect a;

    vec2 bPos;
    Rect b;
};

MyData *Data = NULL;

void MyInit() {
    Game->myData = malloc(sizeof(MyData));
    memset(Game->myData, 0, sizeof(MyData));
    
    Data = (MyData *)Game->myData;

    Data->aPos = V2(-4, 0);
    Data->a.min = V2(-1, -1);
    Data->a.max = V2(1, 1);

    Data->bPos = V2(0);
    Data->b.min = V2(-0.5f, -0.5f);
    Data->b.max = V2(0.5f, 0.5f);
}

void MyGameUpdate() {

    vec2 bDir = V2(0);
    if (InputHeld(Input, Input_Up)) {
        bDir.y = 1;
    }
    if (InputHeld(Input, Input_Down)) {
        bDir.y = -1;
    }
    if (InputHeld(Input, Input_Right)) {
        bDir.x = 1;
    }
    if (InputHeld(Input, Input_Left)) {
        bDir.x = -1;
    }

    r32 speed = 8.0f;
    Data->bPos = Data->bPos + (bDir * speed * Game->deltaTime);
    //Log();

    // @TODO: what if we wanted b to push a?
    vec2 outDir = V2(0);

    if (RectTest(Data->b, Data->a, Data->bPos, Data->aPos, &outDir)) {
        Data->bPos = Data->bPos + outDir;
    }
    
    vec2 aScale = (Data->a.max - Data->a.min) * 0.5f;
    DrawRect(Data->aPos, aScale, RED);

    vec2 bScale = (Data->b.max - Data->b.min) * 0.5f;
    DrawRect(Data->bPos, bScale, BLUE);
}
