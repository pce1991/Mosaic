
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


int32 PlaySound(AudioPlayer *player, SoundClip clip, real32 volume, bool loop) {
    // TODO: FREELIST
    
    PlayingSound sound = {};
    sound.playing = true;
    sound.clip = clip;
    sound.loop = loop;
    sound.volume = volume;

    // @MAYBE: is this right?
    sound.samplesInBuffer = clip.sampleCount;
    int32 index = PushBack(&player->playingSounds, sound);

    return index;
}

int32 PlaySound(AudioPlayer *player, SoundClip clip, real32 volume) {
    return PlaySound(player, clip, volume, false);
}


void PlayAudio(AudioPlayer *player, int32 samplesToRender, real32 *output) {
    memset(output, 0, sizeof(real32) * samplesToRender * 2);
    
    for (int i = 0; i < player->playingSounds.count; i++) {
        PlayingSound *sound = &player->playingSounds[i];
        if (!sound->playing) { continue; }

        int32 samplesToRenderForSound = samplesToRender;
        if (sound->samplesRendered + samplesToRender > sound->samplesInBuffer) {
            samplesToRenderForSound = sound->samplesInBuffer - sound->samplesRendered;
        }

        for (int i = 0; i < samplesToRenderForSound; i++) {
            output[2 * i] += sound->clip.data[i + sound->samplesRendered] * sound->volume;
            output[(2 * i) + 1] += sound->clip.data[i + sound->samplesRendered] * sound->volume;
        }

        sound->samplesRendered += samplesToRender;

        if (sound->samplesRendered >= sound->samplesInBuffer) {
            if (sound->loop) {
                sound->samplesRendered = 0;    
            }
            else {
                sound->playing = false;
                // FREELIST
            }
        }
    }
}
