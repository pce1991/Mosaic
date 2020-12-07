
struct MyData {
    Sprite sprite;
    Sprite sprite2;
};

MyData *Data = NULL;

Sprite lemonSprite;

void MyInit() {
    Game->myData = malloc(sizeof(MyData));
    memset(Game->myData, 0, sizeof(MyData));

    Data = (MyData *)Game->myData;

    // All sprites must be png files
    // You provide the path to the png like "directory/to/my/file/file_name.png"
    // In this case I have put the pngs inside the "data" directory.
    LoadSprite(&Data->sprite, "data/galaga_ship.png");
    LoadSprite(&Data->sprite2, "data/van_gogh.png");

    LoadSprite(&lemonSprite, "data/bad_lemon.png");
}


vec2 position = V2(1, 4);

vec2 scale = V2(1, 1);

vec2 lemonPosition = V2(0, 0);

void MyGameUpdate() {
    // This sets the background color. 
    ClearColor(RGB(0.2f, 0.0f, 0.2f));
    
    // version that doesnt take an angle.
    DrawSprite(V2(0), V2(4, 4), &Data->sprite2);
    DrawSprite(lemonPosition, V2(1, 1), Game->time, &Data->sprite);

    if (InputHeld(Input, Input_Up)) {
        lemonPosition.y += 2 * Game->deltaTime;
    }

    // position in pixels
    vec2 mousePos = Input->mousePosNormSigned;
    mousePos.x = mousePos.x * 8;
    mousePos.y = mousePos.y * 4.5f;
    DrawSprite(mousePos, V2(0.5f, 0.5f), &lemonSprite);

    DrawRect(position, V2(1, 1), RGB(1.0f, 0.3f, 0.3f));

    //DrawRect(V2(-2, -2), scale, RGB(0, 1, 1));

    DrawRect(V2(-2, -2), scale, V4(0.0f, 1.0f, 1.0f, 0.5f));

    
    scale.x -= 0.2f * Game->deltaTime;
    scale.y -= 0.2f * Game->deltaTime;
    // once scale goes negative we will have inverted the shape,
    // it will continue to grow.

    position.y -= 2 * Game->deltaTime;

    // things are drawn in the order you call the functions.
    // The latest draw call will be on top of all previous, and so on.

    // (0, 0) is center of the screen
    // increasing values of y move up
    // We have negative coordinates
    // The width of our screen is 16 (-8 to 8) (left to right)
    // The height of our screen is 9 (-4.5 to 4.5) (bottom to top)
}
