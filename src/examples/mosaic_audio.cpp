

struct MyData {
    SoundClip sound;

    int32 soundIndex;
};

MyData *Data = NULL;

void MyMosaicInit() {
    Data = (MyData *)malloc(sizeof(MyData));
    memset(Data, 0, sizeof(MyData));

    Data->soundIndex = -1;

    SetMosaicGridSize(8, 8);
    
    LoadSoundClip("data/sfx/flute_breathy_c4.wav", &Data->sound);
}

void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    PlayingSound *playingSound = NULL;
    r32 soundT = 0.0f;
    
    if (Data->soundIndex >= 0) {
        playingSound = &Game->audioPlayer.playingSounds[Data->soundIndex];

        if (playingSound && playingSound->playing) {
            soundT = playingSound->samplesRendered / (1.0f * playingSound->clip.sampleCount);
        }
    }

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = V4(0.0f, 0.0f, 0.0f, 1.0f);

            tile->color = Lerp(BLACK, RED, soundT);

            if (playingSound) {
                tile->color.g += playingSound->volume;
            }
        }
    }

    if (playingSound) {
        playingSound->volume = Lerp(0.0f, 2.0f, Mosaic->hoveredTile->position.y / (1.0f * Mosaic->gridHeight));
    }

    if (Data->soundIndex <= 0) {
        Data->soundIndex = PlaySound(&Game->audioPlayer, Data->sound, 1.0f, true);
    }
}
