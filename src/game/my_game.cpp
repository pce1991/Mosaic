
DynamicArray<int32> test;

struct MyData {
    Sprite sprite;
    Sprite sprite2;

    SoundClip sound;
};

MyData Data = {};

Sprite lemonSprite;

void MyGameInit() {
    
    // All sprites must be png files
    // You provide the path to the png like "directory/to/my/file/file_name.png"
    // In this case I have put the pngs inside the "data" directory.
    LoadSprite(&Data.sprite, "data/textures/galaga_ship.png");
    LoadSprite(&Data.sprite2, "data/textures/van_gogh.png");

    LoadSprite(&lemonSprite, "data/textures/bad_lemon.png");

    LoadSoundClip("data/sfx/flute_breathy_c4.wav", &Data.sound);
}


vec2 position = V2(5, 0);

vec2 scale = V2(1, 1);

void MyGameUpdate() {
    // This sets the background color. 
    ClearColor(RGB(0.0f, 0.0f, 0.0f));

    if (InputHeld(Keyboard, Input_UpArrow)) {
        position.y += 2 * Game->deltaTime;
    }

    //Game->camera.position = V3(0, 0, 3) + V3(sinf(Time), 0, 0);
    Game->camera.size = 1 + (((1 + sinf(Time)) * 0.5f) * 0.5f);

    // Always make sure you call UpdateCamera so it can recompute the projection matrix
    UpdateCamera(&Game->camera);
    
    if (InputPressed(Keyboard, Input_Space)) {
        PlaySound(&Game->audioPlayer, Data.sound, 1.0f, true);
    }
    
    DrawSprite(V2(0), V2(4, 4), &Data.sprite2);

    // version that take an angle.
    //DrawSprite(V2(0), V2(4, 4), DegToRad(Time * 90), &Data.sprite2);

    // showing a few different ways to use mouse input.
    // You can get the worldPosition of the mouse which is calculated
    // internally by raycasting onto a plane at the origin of the world
    // pointing towards camera. 
    DrawRect(Input->mousePosWorld, V2(0.5f, 0.5f), V4(1));

    // You could also use the pixel coordinates of the mouse.
    // Note that the origin of the rectangle here is at the top-left,
    // unlike DrawRect which centers the rectangle. 
    //DrawRectScreen(V2(Input->mousePos.x, Input->mousePos.y), V2(32, 32), V4(1));

    // Or use the normalized coordinates.
    //DrawRectScreenNorm(Input->mousePosNorm, V2(0.1f, 0.1f), V4(1));
    
    DrawRect(V2(-7, 0), V2(1, 1), RGB(1.0f, 0.3f, 0.3f));
    DrawSprite(position, V2(0.5f, 0.5f), &Data.sprite);

    DrawLine(V2(3, 0), V2(4, 0), 0.025f, V4(1));
    DrawLine(V2(3, 0), V2(3, 1), 0.025f, V4(1,0,0,1));

    DrawRay(V2(-5, -5), Rotate(Time, V2(1, 0)), 0.025f, V4(0.0f, 0.5f, 0.8f, 1.0f));

    DrawCircle(V2(-5, -5), 1.0f, 0.025f, V4(1));
}
