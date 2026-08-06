
#ifndef SYNTH_H
#define SYNTH_H

// A simple Pico-8-style synthesizer.
//
// An Instrument is a description of a sound: a waveform and an ADSR
// envelope. You can play a note by calling PlayNote() with a frequency
// (in Hz), a volume, and a duration (in seconds). The note plays through
// its attack, decay, and sustain stages for `duration` seconds, then
// releases. You never have to think about samples yourself.

#define MAX_SYNTH_VOICES 32

enum WaveformType {
    Waveform_Sine,
    Waveform_Square,
    Waveform_Triangle,
    Waveform_Sawtooth,
    Waveform_Noise,

    // Soft timbres are not separate waveforms: they are Instruments built
    // from one of these basic shapes plus a low-pass cutoff (see lowPass).

    Waveform_Count,
};

struct Instrument {
    WaveformType waveform;

    // 0..1 low-pass filter cutoff. 1 = fully open (default), lower = darker,
    // softer. This is the main knob for taking the harsh edge off a sound.
    real32 lowPass;

    ADSREnvelope envelope;
};

struct SynthNote {
    // Handshake fields shared between the game thread (PlayNote) and the
    // audio thread (MixSynthNotes). Everything else is either written only
    // by PlayNote (waveform, envelope, frequency, volume, lowPass) or only
    // owned by the mixer while active (phase, filterState1/2, noiseSeed,
    // samplesElapsed). volatile + full barriers keep the two sides from
    // observing torn state.
    volatile bool active;
    volatile int32 generation;

    WaveformType waveform;
    ADSREnvelope envelope;

    real32 frequency;
    real32 phase;        // where we are in the wave cycle, in [0, 1)
    real32 volume;

    real32 lowPass;       // 0..1 filter cutoff knob, mapped to Hz (1 = open)
    real32 filterState1;  // running state of the per-note 2-pole low-pass
    real32 filterState2;  //   (state-variable filter: two integrators)

    real64 samplesElapsed;   // how long this note has been playing
    real64 samplesDuration;  // how long the note is held before release

    uint32 noiseSeed;    // deterministic seed for the noise waveform
};

struct SynthPlayer {
    SynthNote notes[MAX_SYNTH_VOICES];
};

void SynthPlayerInit(SynthPlayer *synth);

SoundHandle PlayNote(SynthPlayer *synth, Instrument instrument, real32 frequency,
                     real32 volume, real32 durationSeconds);

void MixSynthNotes(SynthPlayer *synth, int32 samplesToRender, real32 *output);

real32 WaveformSample(WaveformType type, real32 phase, uint32 *noiseSeed);

#endif // SYNTH_H
