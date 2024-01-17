// Manual can be found here:
// https://github.com/pce1991/MosaicBook#readme

struct MyData {
    Sprite sprite;
    Sprite sprite2;

    SoundClip sound;
};

MyData Data = {};

Sprite lemonSprite;

void MyInit() {
    // All sprites must be png files
    // You provide the path to the png like "directory/to/my/file/file_name.png"
    // In this case I have put the pngs inside the "data" directory.
    LoadSprite(&Data.sprite, "data/galaga_ship.png");
    LoadSprite(&Data.sprite2, "data/van_gogh.png");

    LoadSprite(&lemonSprite, "data/bad_lemon.png");

    LoadSoundClip("data/sfx/flute_breathy_c4.wav", &Data.sound);
}


vec2 position = V2(4, 0);

vec2 scale = V2(1, 1);

void MyGameUpdate() {
    // This sets the background color. 
    ClearColor(RGB(0.0f, 0.0f, 0.0f));

    if (InputHeld(Keyboard, Input_UpArrow)) {
        position.y += 2 * Game->deltaTime;
    }

    //Game->camera.position = V3(0, 0, 3) + V3(sinf(Time), 0, 0);
    Game->camera.size = 1 + ((1 + sinf(Time)) * 0.5f);

    // Always make sure you call UpdateCamera so it can recompute the projection matrix
    UpdateCamera(&Game->camera);
    
    
    // position in pixels
    vec2 mousePos = Input->mousePosNormSigned;
    mousePos.x = mousePos.x * (Game->camera.width * Game->camera.size * 0.5f);
    mousePos.y = mousePos.y * (Game->camera.height * Game->camera.size * 0.5f);
        
    mousePos = mousePos + Game->camera.position.xy;
    
    if (InputPressed(Keyboard, Input_Space)) {
        PlaySound(&Game->audioPlayer, Data.sound, 1.0f, true);
    }
    
    DrawSprite(V2(0), V2(4, 4), &Data.sprite2);

    // version that take an angle.
    DrawSprite(mousePos, V2(0.5f, 0.5f), DegToRad(Time * 90), &lemonSprite);
    DrawRect(V2(0, 0), V2(1, 1), RGB(1.0f, 0.3f, 0.3f));
    DrawSprite(position, V2(0.5f, 0.5f), &Data.sprite);
}
