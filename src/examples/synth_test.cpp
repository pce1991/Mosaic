
// synth_test.cpp
// A demo of the built-in synthesizer. An Instrument is a waveform plus an
// ADSR envelope and a low-pass filter. PlayNote() turns it into sound at
// runtime -- no wav files needed.
//
//   Keys A-L        play notes (a C major scale starting at C4)
//   Keys W,E,T,Y,U  the black keys in between
//   Keys 1-5        switch the lead shape: sine/square/triangle/sawtooth/noise
//   Keys 6-8        switch to soft presets: soft square / soft saw / organ
//   Key 9           angel vox (soft-saw pad, detuned double layer)
//   Space           play a noise hit
//
//   A little melody plays itself in a loop: a soft square lead, a triangle
// bass, and filtered noise hats on the off-beats. The tiles light up for
// each playing note, colored by waveform, with pitch shown by height.

static r32 NoteFrequency(int32 midiNote) {
    return 440.0f * powf(2.0f, (midiNote - 69) / 12.0f);
}

static Instrument Lead;
static Instrument Bass;
static Instrument Hat;
static Instrument AngelicVox;
static Instrument SoftSquare;
static Instrument SoftSaw;
static Instrument OrganPreset;
static bool VoxEnabled = false;

// lead melody: C, D, Em, F -- sixteenth notes at 120 bpm
static r32 LeadMelody[] = {
    NoteFrequency(60), NoteFrequency(64), NoteFrequency(67), NoteFrequency(64),
    NoteFrequency(60), NoteFrequency(64), NoteFrequency(67), NoteFrequency(64),
    NoteFrequency(62), NoteFrequency(65), NoteFrequency(69), NoteFrequency(65),
    NoteFrequency(62), NoteFrequency(65), NoteFrequency(69), NoteFrequency(65),
    NoteFrequency(64), NoteFrequency(67), NoteFrequency(71), NoteFrequency(67),
    NoteFrequency(64), NoteFrequency(67), NoteFrequency(71), NoteFrequency(67),
    NoteFrequency(65), NoteFrequency(69), NoteFrequency(72), NoteFrequency(69),
    NoteFrequency(65), NoteFrequency(69), NoteFrequency(72), NoteFrequency(69),
};
static int32 LeadIndex = 0;
static r32 LastLeadTime = 0;

// bass plays the root of each beat
static r32 BassMelody[] = {
    NoteFrequency(48), NoteFrequency(50), NoteFrequency(52), NoteFrequency(53),
};
static int32 BassIndex = 0;
static r32 LastBassTime = 0;

static r32 Sixteenth = 0.25;

static const char *WaveformNames[] = {
    "sine", "square", "triangle", "sawtooth", "noise",
};

static vec4 WaveformColor(WaveformType type) {
    switch (type) {
        case Waveform_Sine:        return RGB(1.0f, 0.3f, 0.3f);
        case Waveform_Square:      return RGB(0.3f, 1.0f, 0.3f);
        case Waveform_Triangle:    return RGB(0.3f, 0.6f, 1.0f);
        case Waveform_Sawtooth:    return RGB(1.0f, 1.0f, 0.3f);
        case Waveform_Noise:       return RGB(0.9f, 0.9f, 0.9f);
        default:                   return RGB(1.0f, 1.0f, 1.0f);
    }
}

void MyMosaicInit() {
    // The lead defaults to a soft square: a square through a low-pass cutoff.
    // lowPass is a cutoff knob (0..1 maps to 20 Hz .. 20 kHz, exponentially).
    Lead.waveform = Waveform_Square;
    Lead.lowPass = 0.75f;
    Lead.envelope.attack = 0.15f;
    Lead.envelope.decay = 0.16f;
    Lead.envelope.sustainAmp = 0.28f;
    Lead.envelope.release = 0.12f;
    Lead.envelope.startAmp = 1.0f;

    Bass.waveform = Waveform_Triangle;
    Bass.lowPass = 0.6f;
    Bass.envelope.attack = 0.015f;
    Bass.envelope.decay = 0.2f;
    Bass.envelope.sustainAmp = 0.4f;
    Bass.envelope.release = 0.2f;
    Bass.envelope.startAmp = 1.0f;

    Hat.waveform = Waveform_Noise;
    Hat.lowPass = 0.55f;
    Hat.envelope.attack = 0.002f;
    Hat.envelope.decay = 0.06f;
    Hat.envelope.sustainAmp = 0.0f;
    Hat.envelope.release = 0.03f;
    Hat.envelope.startAmp = 1.0f;

    // angelic vox: a soft-saw pad with a slow swell and a long, gentle tail.
    // Classic choral character comes from the detuned double layer in
    // PlayLeadNote(), not from any engine feature.
    AngelicVox.waveform = Waveform_Sawtooth;
    AngelicVox.lowPass = 0.75f;
    AngelicVox.envelope.attack = 0.5f;
    AngelicVox.envelope.decay = 0.3f;
    AngelicVox.envelope.sustainAmp = 0.7f;
    AngelicVox.envelope.release = 1.0f;
    AngelicVox.envelope.startAmp = 1.0f;

    // Soft presets: a basic shape plus a cutoff. The old additive "soft
    // square", "soft saw", and "organ" timbres live here as instruments now.
    SoftSquare.waveform = Waveform_Square;
    SoftSquare.lowPass = 0.75f;
    SoftSquare.envelope = Lead.envelope;

    SoftSaw.waveform = Waveform_Sawtooth;
    SoftSaw.lowPass = 0.65f;
    SoftSaw.envelope = Lead.envelope;

    OrganPreset.waveform = Waveform_Square;
    OrganPreset.lowPass = 0.85f;
    OrganPreset.envelope.attack = 0.02f;
    OrganPreset.envelope.decay = 0.1f;
    OrganPreset.envelope.sustainAmp = 0.5f;
    OrganPreset.envelope.release = 0.1f;
    OrganPreset.envelope.startAmp = 1.0f;
}

// Lead is either the normal plucky instrument or the angel vox pad. In vox
// mode each note is two slightly-detuned layers (~5 cents apart) so the pad
// gets that choral "movement", and keyboard notes are held much longer.
static void PlayLeadNote(SynthPlayer *synth, r32 frequency, r32 volume, r32 duration, r32 heldDuration) {
    if (VoxEnabled) {
        PlayNote(synth, AngelicVox, frequency * 1.003f, 0.15f, heldDuration);
        PlayNote(synth, AngelicVox, frequency * 0.997f, 0.15f, heldDuration);
    }
    else {
        PlayNote(synth, Lead, frequency, volume, duration);
    }
}

// A raw lead: keep the current envelope but hear the basic shape unfiltered.
static Instrument RawLead(WaveformType waveform) {
    Instrument result = Lead;
    result.waveform = waveform;
    result.lowPass = 1.0f;
    return result;
}

void MyMosaicUpdate() {
    ClearTiles(V4(0));

    SynthPlayer *synth = Core->audioPlayer.synth;

    // melody: lead on every sixteenth, bass on every beat, hats on off-beats
    if (Time - LastLeadTime >= Sixteenth * 2) {
        LastLeadTime = Time;
        PlayLeadNote(synth, LeadMelody[LeadIndex], 0.25f, 0.5f, 0.5f);
        LeadIndex = (LeadIndex + 1) % ArrayLength(LeadMelody, r32);

        if (LeadIndex % 2 == 1) {
            PlayNote(synth, Hat, 1.0f, 0.12f, 0.05f);
        }
    }

    if (Time - LastBassTime >= 4 * Sixteenth) {
        LastBassTime = Time;
        PlayNote(synth, Bass, BassMelody[BassIndex], 0.18f, 0.4f);
        BassIndex = (BassIndex + 1) % ArrayLength(BassMelody, r32);
    }

    // play notes with the keyboard
    InputKeyboardDiscrete keyRow[] = {
        Input_A, Input_S, Input_D, Input_F, Input_G, Input_H, Input_J, Input_K, Input_L,
    };
    int32 keyMidi[] = {
        60, 62, 64, 65, 67, 69, 71, 72, 74,
    };
    for (int32 i = 0; i < ArrayLength(keyRow, InputKeyboardDiscrete); i++) {
        if (InputPressed(Keyboard, keyRow[i])) {
            PlayLeadNote(synth, NoteFrequency(keyMidi[i]), 0.25f, 0.4f, 1.5f);
        }
    }

    InputKeyboardDiscrete blackKeyRow[] = {
        Input_W, Input_E, Input_T, Input_Y, Input_U,
    };
    int32 blackKeyMidi[] = {
        61, 63, 66, 68, 70,
    };
    for (int32 i = 0; i < ArrayLength(blackKeyRow, InputKeyboardDiscrete); i++) {
        if (InputPressed(Keyboard, blackKeyRow[i])) {
            PlayLeadNote(synth, NoteFrequency(blackKeyMidi[i]), 0.25f, 0.4f, 1.5f);
        }
    }

    // 1-5: raw basic shapes (filter fully open). 6-8: soft presets.
    if (InputPressed(Keyboard, Input_1)) { VoxEnabled = false; Lead = RawLead(Waveform_Sine); }
    if (InputPressed(Keyboard, Input_2)) { VoxEnabled = false; Lead = RawLead(Waveform_Square); }
    if (InputPressed(Keyboard, Input_3)) { VoxEnabled = false; Lead = RawLead(Waveform_Triangle); }
    if (InputPressed(Keyboard, Input_4)) { VoxEnabled = false; Lead = RawLead(Waveform_Sawtooth); }
    if (InputPressed(Keyboard, Input_5)) { VoxEnabled = false; Lead = RawLead(Waveform_Noise); }
    if (InputPressed(Keyboard, Input_6)) { VoxEnabled = false; Lead = SoftSquare; }
    if (InputPressed(Keyboard, Input_7)) { VoxEnabled = false; Lead = SoftSaw; }
    if (InputPressed(Keyboard, Input_8)) { VoxEnabled = false; Lead = OrganPreset; }
    if (InputPressed(Keyboard, Input_9)) { VoxEnabled = true; }

    if (InputPressed(Keyboard, Input_Space)) {
        PlayNote(synth, Hat, 1.0f, 0.15f, 0.05f);
    }

    // visualize: one tile per active note, colored by waveform, height = pitch
    for (int32 i = 0; i < MAX_SYNTH_VOICES; i++) {
        SynthNote *note = &synth->notes[i];
        if (!note->active) { continue; }

        r32 midi = 69.0f + 12.0f * log2f(note->frequency / 440.0f);
        int32 y = Clamp(72 - (int32)midi, 0, Mosaic->gridHeight - 1);

        SetTileColor(i % Mosaic->gridWidth, y, WaveformColor(note->waveform));
    }

    const char *leadName = VoxEnabled ? "angel vox" : WaveformNames[Lead.waveform];
    DrawTextTop(V4(1), "waveform: %s   |   A-L play notes, 1-5 shapes, 6-8 soft presets, 9 = angel vox, space = noise",
                leadName);
}
