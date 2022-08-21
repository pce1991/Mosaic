
SoundClip sound;
SoundHandle handle;

void MyMosaicInit() {
    LoadSoundClip("data/sfx/flute_breathy_c4.wav", &sound);

    Mosaic->gridOrigin.x += -6;
}

void MyMosaicUpdate() {
    if (handle.generation == 0) {
        handle = PlaySound(&Game->audioPlayer, sound, 1.0f, true);
    }

    if (Sound *sound = GetSound(&Game->audioPlayer, handle)) {
        sound->volume = (1 + sinf(Time * 5)) * 0.5f;

        vec2 gridHalfSize = Mosaic->gridSize * 0.5f;
        vec2 gridTopLeft = Mosaic->gridOrigin;
        vec2 position = gridTopLeft + V2(gridHalfSize.x, 0);

        //DrawTextTop(V4(1), "volume %.2f", sound->volume);
    }

    DrawText(&Game->monoFont, V2(0, 0), 1.0f, V4(1), false, "Abc");
    DrawTextScreenPixel(&Game->monoFont, V2(400, 400), 64.0f, V4(1), false, "Abc");

    real32 textSize = 0.5f;
    real32 lineHeight = Game->monoFont.lineHeight * textSize;
    
    vec2 textCursor = Mosaic->gridOrigin;
    textCursor.x += Mosaic->gridSize.x;
    textCursor.y -= 1;

    DrawText(&Game->monoFont, textCursor, textSize, V4(1), false, "Abc");
    textCursor.y -= 1.0f;

    DrawText(&Game->monoFont, textCursor, textSize, V4(1), false, "efg");
    textCursor.y -= 1.0f;

    DrawText(&Game->monoFont, textCursor, textSize, V4(1), false, "hij");
    textCursor.y -= 1.0f;

    int32 index = 0;
    
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            if (index % 2 == 0) {
                SetTileColor(x, y, 0.3f, 0.0f, 0.0f);
            }
            else {
                SetTileColor(x, y, 0, 0, 0);
            }

            index++;
        }

        index++;
    }
}
