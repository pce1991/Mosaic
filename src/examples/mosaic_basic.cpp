
SoundClip sound;
SoundHandle handle;

void MyMosaicInit() {
    LoadSoundClip("data/sfx/flute_breathy_c4.wav", &sound);
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
                
        //DrawText(&Game->serifFont, position, 1.0f, V4(1), true, "volume %.2f", sound->volume);
        DrawTextTop(V4(1), "volume %.2f", sound->volume);
    }

    
}
