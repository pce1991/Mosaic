
#define WAV_RIFF_VALUE 0x46464952
#define WAV_WAVE_VALUE 0x45564157
#define WAV_FMT_VALUE 0x20746d66
#define WAV_DATA_VALUE 0x61746164

enum WavFormat {
    WAV_PCM                = 0x01,
    WAV_IEEEFloatingPoint  = 0x03,
    WAV_ALaw               = 0x06,
    WAV_MuLaw              = 0x07,
    WAV_IMAADPCM           = 0x11,
    WAV_YamahaITUG723ADPCM = 0x16,
    WAV_GSM610             = 0x31,
    WAV_ITUG721ADPCM       = 0x40,
    WAV_MPEG               = 0x50,
    WAV_Extensible         = 0xFFFE
};

// Sound clip must be a .wav file and must be a mono track (or a stereo I forget lol)
bool LoadSoundClip(const char *audioPath, SoundClip *sound) {
    bool success = true;
    
    FILE *file = fopen(audioPath, "rb");
    
    if (file != NULL) {
        WavFormat format;

        int32 riffID;
        fread(&riffID, sizeof(int32), 1, file);
        int32 chunkSize;
        fread(&chunkSize, sizeof(int32), 1, file);
        int32 riffFormat;
        fread(&riffFormat, sizeof(int32), 1, file);

        int32 fileOffset_ = ftell(file);

        if (riffID == WAV_RIFF_VALUE && riffFormat == WAV_WAVE_VALUE) {

            int16 numChannels = 0;
            int16 bitsPerSample = 0;
            int32 sampleRate = 0;
            
            bool atData = false;
            while (!atData) {
                int32 subchunkID;
                fread(&subchunkID, sizeof(int32), 1, file);

                switch (subchunkID) {
                    case WAV_FMT_VALUE: {
                        int32 subchunkSize;
                        fread(&subchunkSize, sizeof(int32), 1, file);
                        int32 fileOffset = ftell(file);
                        uint32 nextSubchunkOffset = fileOffset + (uint32)subchunkSize;

                        int16 formatTemp;
                        fread(&formatTemp, sizeof(int16), 1, file);
                        format = (WavFormat)formatTemp;
                        fread(&numChannels, sizeof(int16), 1, file);
                        fread(&sampleRate, sizeof(int32), 1, file);
                        int32 byteRate;
                        fread(&byteRate, sizeof(int32), 1, file);
                        int16 blockAlign;
                        fread(&blockAlign, sizeof(int16), 1, file);
                        fread(&bitsPerSample, sizeof(int16), 1, file);
                        
                        fseek(file, nextSubchunkOffset, SEEK_SET);
                    } break;
                    case WAV_DATA_VALUE: {
                        atData = true;
                    } break;
                    default: {
                        int32 subchunkSize;
                        fread(&subchunkSize, sizeof(int32), 1, file);
                        fseek(file, subchunkSize, SEEK_CUR);
                    } break;
                }
            }

            ASSERT(numChannels > 0);
            ASSERT(numChannels <= 2);
            ASSERT(sampleRate > 0);

            uint32 dataSizeInBytes;
            fread(&dataSizeInBytes, sizeof(int32), 1, file);
            uint32 sampleCount = dataSizeInBytes / (bitsPerSample / 8) / numChannels;

            sound->sampleRate = sampleRate;
            sound->sampleCount = sampleCount;
            sound->channels = numChannels;

            switch (format) {
                case WAV_PCM: {
                    sound->data = (real32 *)malloc(sizeof(real32) * sampleCount * numChannels);
                
                    // Always read in little-endian!
                    for (int i = 0; i < sampleCount * numChannels; i++) {
                        int16 sample;
                        fread(&sample, sizeof(int16), 1, file);
                        sound->data[i] = sample / (real32)32768.0f;
                        //Print("%d %d %f", i, sample, sound->data[i]);
                    }

                } break;
                default: {
                    success = false;
                } break;
            }
        }

        fclose(file);

        return success;
    }

    return success;
}

/*
  Mixer works by filling out the samples for every buffer
  We write a particular sound into one buffer
  then we add that clip to the buffer of all the nodes for that clip
  
  I think all we need to do is take every sound clip, and just add the notes to our buffer
 */


void GenerateSineWaveClip(SoundClip *clip) {
   clip->sampleCount = HZ;
   clip->channels = 2;

   clip->data = (real32 *)malloc(sizeof(real32) * clip->sampleCount);

   
    ADSREnvelope env = {};
    env.attack = 0.01f;
    env.decay = 0.6f;
    env.sustainAmp = 0.0f;
    env.startAmp = 1.0f;
   
   for (int i = 0; i < clip->sampleCount; i++) {
       clip->data[i] = SineWaveLFO(i, 440.0f, 4.0f, 0.001f) + (SineWave(i, 880.0f) * 0.5f) + (SineWave(i, 1720.0f) * 0.25f);
       real32 time = (i * 1.0f) / HZ;
       clip->data[i] *= 0.5f;
       clip->data[i] *= CalculateAmplitude(env, time);
   }
}


SoundHandle PlaySound(AudioPlayer *player, SoundClip clip, real32 volume, bool loop) {
    Sound *sound = NULL;
    
    int32 index;
    if (PopBack(&player->freeList, &index)) {
        sound = &player->sounds[index];
        *sound = {};
    }
    else {
        sound = PushBackPtr(&player->sounds);
        index = player->sounds.count - 1;
        sound->generation = 1;
    }

    sound->playing = true;
    sound->clip = clip;
    sound->loop = loop;
    sound->volume = volume;

    // @MAYBE: is this right?
    sound->samplesInBuffer = clip.sampleCount;

    SoundHandle handle = {};
    handle.index = index;
    handle.generation = sound->generation;

    return handle;
}

SoundHandle PlaySound(AudioPlayer *player, SoundClip clip, real32 volume) {
    return PlaySound(player, clip, volume, false);
}

Sound *GetSound(AudioPlayer *player, SoundHandle handle) {
    if (handle.index > player->sounds.count) {
        return NULL;
    }
    
    Sound *sound = &player->sounds[handle.index];

    if (sound->generation != handle.generation) {
        return NULL;
    }

    return sound;
}

void AudioPlayerInit(AudioPlayer *player, MemoryArena *arena) {
    player->sounds = MakeDynamicArray<Sound>(arena, 128);
    player->freeList = MakeDynamicArray<int32>(arena, 128);
}

void PlayAudio(AudioPlayer *player, int32 samplesToRender, real32 *output) {
    memset(output, 0, sizeof(real32) * samplesToRender * 2);

    // Even tho main thread can add things to the sounds buffer as it's being
    // executed, nothing else is allowed to touch that data,
    // @BUG: technically this buffer may grow as we're addying things to it
    // because the DynamicArray may resize. So we should really use a fixed size
    // buffer of the max number of sounds.
    
    for (int i = 0; i < player->sounds.count; i++) {
        Sound *sound = &player->sounds[i];
        if (!sound->playing) { continue; }

        int32 samplesToRenderForSound = samplesToRender;
        if (sound->samplesRendered + samplesToRender > sound->samplesInBuffer) {
            samplesToRenderForSound = sound->samplesInBuffer - sound->samplesRendered;
        }

        r32 startVolume = sound->lastVolume;
        r32 targetVolume = sound->volume;
        
        for (int i = 0; i < samplesToRenderForSound; i++) {
            r32 volume = Lerp(startVolume, targetVolume, i / (samplesToRenderForSound * 1.0f));
            output[2 * i] += sound->clip.data[i + sound->samplesRendered] * volume;
            output[(2 * i) + 1] += sound->clip.data[i + sound->samplesRendered] * volume;
        }

        sound->lastVolume = targetVolume;

        sound->samplesRendered += samplesToRender;

        if (sound->samplesRendered >= sound->samplesInBuffer) {
            if (sound->loop) {
                sound->samplesRendered = 0;    
            }
            else {
                sound->playing = false;
                sound->generation++;
                PushBack(&player->freeList, i);
            }
        }
    }
}
