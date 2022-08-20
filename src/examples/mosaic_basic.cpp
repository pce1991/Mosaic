
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
    }
}
