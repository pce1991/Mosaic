
#define HZ 44100

struct SoundClip {
    uint32 sampleRate;
    uint32 channels;
    uint32 sampleCount;

    // channels are interleaved
    real32 *data;
};

struct Sound {
    SoundClip clip;
    // @NOTE: this data is copied from our soundClip, but it doesnt allocate the data of a sound clip
    // (nor should it), but that means you cant do anything to it without messing with the source.

    bool playing;
    bool loop;

    real32 volume;

    // @NOTE: do not touch this value. This is for the mixing so we interpolate
    // between the volume at the last frame and the new volume we want to render at.
    real32 lastVolume;

    uint32 samplesInBuffer;
    uint32 samplesRenderedFromBuffer;
    uint64 samplesRendered;

    // @TODO: start time

    int32 generation;
};

struct AudioPlayer {
    real32 volume;
    
    DynamicArray<Sound> sounds;
    DynamicArray<int32> freeList;
};

struct SoundHandle {
    int32 index;
    int32 generation; // a generation of 0 is invalid
};

inline real32 SineWave(real32 time, real32 freq) {
    real32 step = HZ / freq;
    real32 cycles = time / step;
    real32 rad = _2PI * cycles;
    return sinf(rad);
}

real32 SineWaveLFO(real32 time, real32 freq, real32 ampLFO, real32 freqLFO) {
    real32 step = HZ / freq;
    real32 cycles = time / step;
    real32 rad = _2PI * cycles;

    r32 stepLFO = HZ / freqLFO;
    r32 cyclesLFO = time / stepLFO;
    r32 radLFO = _2PI * cyclesLFO;
    
    return sinf(rad + ampLFO * freq * sinf(radLFO));
}

struct ADSREnvelope {
    real32 attack;
    real32 decay;
    real32 sustain;
    real32 release;

    real32 startAmp;
    real32 sustainAmp;
};

real32 CalculateAmplitude(ADSREnvelope env, real32 time) {
    real32 amp = env.sustainAmp;

    if (time <= env.attack) {
        amp = (time / env.attack) * env.startAmp;
    }
    if (time > env.attack && time <= (env.attack + env.decay)) {
        amp = ((time - env.attack) / env.decay) * (env.sustainAmp - env.startAmp) + env.startAmp;
    }
    if (time > (env.attack + env.decay)) {
        amp = env.sustainAmp;
    }
    // @TODO: the rest of it

    return amp;
}
