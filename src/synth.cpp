

// See synth.h for a description of the API.
//
// Each note is one voice: a phase accumulator that advances by
// frequency / HZ per sample, a waveform function that turns the phase into
// an amplitude in [-1, 1], and an ADSR envelope that shapes the loudness
// of the note over time. MixSynthNotes() adds every active voice into the
// output buffer in the same way the clip mixer in audio.cpp does.

#include "synth.h"


// Full memory barrier used by the PlayNote / MixSynthNotes handshake.
// Release side: orders every field write before the active store. Acquire
// side: orders the field reads after the active load. Both sides use it so
// the exact same source works on Windows and Linux.
#if _WIN32
#define SynthMemoryBarrier() MemoryBarrier()
#else
#define SynthMemoryBarrier() __sync_synchronize()
#endif


real32 WaveformSample(WaveformType type, real32 phase, uint32 *noiseSeed) {
    real32 result = 0.0f;

    switch (type) {
        case Waveform_Sine: {
            result = sinf(_2PI * phase);
        } break;

        case Waveform_Square: {
            result = (phase < 0.5f) ? 1.0f : -1.0f;
        } break;

        case Waveform_Triangle: {
            result = 1.0f - 4.0f * fabsf(phase - 0.5f);
        } break;

        case Waveform_Sawtooth: {
            result = 2.0f * phase - 1.0f;
        } break;

        case Waveform_Noise: {
            // simple LCG, seeded per note so the same note sounds the same every time
            *noiseSeed = *noiseSeed * 1664525u + 1013904223u;
            result = ((*noiseSeed >> 8) & 0xFFFF) / 32768.0f - 1.0f;
        } break;

        case Waveform_Count:
        default: {
            // no sound
        } break;
    }

    return result;
}

// Returns the envelope amplitude for a note that has been playing for
// `elapsed` seconds and is held for `noteDuration` seconds.
//
//   attack  -- ramp up to startAmp
//   decay   -- fall from startAmp down to sustainAmp
//   sustain -- hold at sustainAmp until noteDuration
//   release -- fade out (starting from whatever the amp was at noteDuration)
static real32 SynthEnvelopeAmplitude(ADSREnvelope env, real64 elapsed, real64 noteDuration) {
    real32 t = (real32)elapsed;
    real32 startAmp = (env.startAmp > 0.0f) ? env.startAmp : 1.0f;

    if (t < env.attack) {
        if (env.attack > 0.0f) {
            // curved attack: starts gently, so onsets don't click
            real32 tN = t / env.attack;
            return tN * tN * startAmp;
        }
        return startAmp;
    }

    if (t < env.attack + env.decay) {
        if (env.decay > 0.0f) {
            return Lerp(startAmp, env.sustainAmp, (t - env.attack) / env.decay);
        }
        return env.sustainAmp;
    }

    if (t < noteDuration) {
        return env.sustainAmp;
    }

    if (env.release <= 0.0f) {
        return 0.0f;
    }

    real32 releaseT = t - (real32)noteDuration;
    if (releaseT >= env.release) {
        return 0.0f;
    }

    // start the release from the amplitude we had when the note was released,
    // so a note that never reached its sustain level doesn't jump in volume.
    real32 ampAtRelease = env.sustainAmp;
    if (noteDuration < env.attack) {
        ampAtRelease = startAmp;
    }
    else if (noteDuration < env.attack + env.decay) {
        ampAtRelease = Lerp(startAmp, env.sustainAmp, ((real32)noteDuration - env.attack) / env.decay);
    }

    // curved release: falls off quickly at first, then tails out gently
    real32 rN = releaseT / env.release;
    return ampAtRelease * (1.0f - rN * rN);
}


void SynthPlayerInit(SynthPlayer *synth) {
    memset(synth, 0, sizeof(SynthPlayer));
}


SoundHandle PlayNote(SynthPlayer *synth, Instrument instrument, real32 frequency,
                     real32 volume, real32 durationSeconds) {
    SoundHandle result = {};
    result.generation = 0; // invalid

    if (synth == NULL) { return result; }
    if (frequency <= 0.0f || volume <= 0.0f || durationSeconds <= 0.0f) { return result; }

    int32 index = -1;
    for (int32 i = 0; i < MAX_SYNTH_VOICES; i++) {
        if (!synth->notes[i].active) {
            index = i;
            break;
        }
    }

    // All voices are busy: steal the one that has been playing the longest.
    if (index == -1) {
        int32 oldest = 0;
        for (int32 i = 1; i < MAX_SYNTH_VOICES; i++) {
            if (synth->notes[i].samplesElapsed > synth->notes[oldest].samplesElapsed) {
                oldest = i;
            }
        }
        index = oldest;
    }

    SynthNote *note = &synth->notes[index];

    // Claim the voice. PlayNote runs on the game thread while MixSynthNotes
    // runs on the audio thread, so a voice must never be visible half-written.
    // The order is: write every field, bump generation (the "this is a fresh
    // claim" marker the mixer checks), then release active=true LAST. The
    // mixer only reads a voice's fields after seeing active==true, and if it
    // detects a generation change mid-block it throws its snapshot away
    // instead of writing back over this fresh state.
    note->waveform = instrument.waveform;
    note->envelope = instrument.envelope;
    note->frequency = frequency;
    note->volume = volume;
    note->lowPass = (instrument.lowPass > 0.0f) ? instrument.lowPass : 1.0f;

    note->phase = 0.0f;
    note->filterState1 = 0.0f;
    note->filterState2 = 0.0f;
    note->samplesElapsed = 0.0;
    note->samplesDuration = (real64)(durationSeconds * HZ);
    note->noiseSeed = 12345u + (uint32)index * 100000u;

    note->generation++;
    SynthMemoryBarrier();
    note->active = true;

    result.index = index;
    result.generation = note->generation;

    return result;
}


void MixSynthNotes(SynthPlayer *synth, int32 samplesToRender, real32 *output) {
    if (synth == NULL) { return; }

    // This runs on the audio thread while PlayNote() runs on the game thread.
    // To avoid seeing a note half-claimed, each voice is rendered from a
    // snapshot taken AFTER the voice was confirmed active, and the snapshot
    // is written back only if generation still matches (i.e. PlayNote didn't
    // re-claim the voice while we were rendering).
    for (int32 i = 0; i < MAX_SYNTH_VOICES; i++) {
        SynthNote *note = &synth->notes[i];

        // acquire the claim: only read fields once active==true is visible
        if (!note->active) { continue; }
        SynthMemoryBarrier();

        int32 generation = note->generation;
        SynthMemoryBarrier();

        WaveformType waveform = note->waveform;
        ADSREnvelope envelope = note->envelope;
        real32 frequency = note->frequency;
        real32 volume = note->volume;
        real32 lowPass = note->lowPass;

        real32 phase = note->phase;
        real32 filterState1 = note->filterState1;
        real32 filterState2 = note->filterState2;
        uint32 noiseSeed = note->noiseSeed;
        real64 samplesElapsed = note->samplesElapsed;
        real64 samplesDuration = note->samplesDuration;

        // Map the lowPass knob (0..1) to a cutoff in Hz with an exponential
        // taper (20 Hz .. 20 kHz), like the cutoff knob on a real synth.
        real32 cutoffHz = 20.0f * powf(1000.0f, lowPass);
        if (cutoffHz > 0.45f * HZ) { cutoffHz = 0.45f * HZ; }

        // 2-pole low-pass (12 dB/oct) in trapezoidal state-variable form:
        // two integrator states per note, unconditionally stable, and ready
        // for a resonance knob later (k = 1/Q; k = 1 gives a flat Q = 1).
        real32 g = tanf(_PI * cutoffHz / HZ);
        real32 k = 1.0f;
        real32 a1 = 1.0f / (1.0f + g * (g + k));
        real32 a2 = g * a1;
        real32 a3 = g * a2;

        for (int32 s = 0; s < samplesToRender; s++) {
            // The envelope times (attack/decay/release) are in seconds, so the
            // sample counts must be converted before asking for the amplitude.
            real64 elapsed = (samplesElapsed + s) * (1.0 / HZ);

            real32 amp = SynthEnvelopeAmplitude(envelope, elapsed, samplesDuration * (1.0 / HZ));
            if (amp <= 0.0f) { break; }

            real32 sample = WaveformSample(waveform, phase, &noiseSeed);
            sample *= amp * volume;

            // The filter states start at 0, so the first samples are also
            // smoothed (kills the attack click).
            real32 v3 = sample - filterState2;
            real32 v1 = a1 * filterState1 + a2 * v3;
            real32 v2 = filterState2 + a2 * filterState1 + a3 * v3;
            filterState1 = 2.0f * v1 - filterState1;
            filterState2 = 2.0f * v2 - filterState2;
            sample = v2;

            output[2 * s] += sample;
            output[2 * s + 1] += sample;

            phase += frequency / HZ;
            if (phase >= 1.0f) { phase -= 1.0f; }
        }

        SynthMemoryBarrier();

        // If generation changed, PlayNote claimed this voice mid-block. The
        // voice now holds a brand-new note whose state we must not clobber.
        if (note->generation != generation) {
            continue;
        }

        note->phase = phase;
        note->filterState1 = filterState1;
        note->filterState2 = filterState2;
        note->noiseSeed = noiseSeed;
        note->samplesElapsed = samplesElapsed + samplesToRender;

        if (note->samplesElapsed >= samplesDuration + (real64)(envelope.release * HZ)) {
            note->active = false;
        }
    }
}
