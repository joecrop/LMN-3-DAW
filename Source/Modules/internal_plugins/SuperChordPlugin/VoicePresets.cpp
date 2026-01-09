#include "VoicePresets.h"

namespace internal_plugins {

VoicePreset VoicePresets::presets[64];
bool VoicePresets::initialized = false;

void VoicePresets::initializePresets() {
    if (initialized)
        return;

    // ============= PADS (1-3) =============

    // Preset 1: "Warm Pad"
    {
        VoicePreset &p = presets[0];
        p.name = "Pad: Warm";
        p.numOscillators = 3;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.7f, 0.0f, 4, 0.03f};
        p.oscillators[1] = {Waveform::Square, 2.0f, 0.3f, 0.0f, 2, 0.01f};
        p.oscillators[2] = {Waveform::Sine, 0.5f, 0.4f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.3f, 0.5f, 0.8f, 1.0f};
        p.filterEnvelope = {0.2f, 0.8f, 0.5f, 0.8f};

        p.filter = {FilterType::LowPass, 2000.0f, 1.5f, 0.3f};

        p.lfo = {Waveform::Sine, 0.3f, 0.05f, true, false, false};

        p.macros[0].name = "Warmth";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 500.0f, 8000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::OscDetune, 0.0f, 0.1f, MacroCurve::Linear});

        p.macros[1].name = "Space";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.0f, 0.5f, MacroCurve::Linear});
        p.macros[1].targets.add({MacroParamType::EnvRelease, 0.3f, 2.0f, MacroCurve::Exponential});

        p.macros[2].name = "Bloom";
        p.macros[2].targets.add({MacroParamType::EnvAttack, 0.01f, 2.0f, MacroCurve::Exponential});
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.1f, 5.0f, MacroCurve::Exponential});
    }

    // Preset 2: "Bright Pad"
    {
        VoicePreset &p = presets[1];
        p.name = "Pad: Bright";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 3, 0.02f};
        p.oscillators[1] = {Waveform::Saw, 2.0f, 0.4f, 0.0f, 2, 0.01f};

        p.ampEnvelope = {0.2f, 0.3f, 0.9f, 0.8f};
        p.filterEnvelope = {0.1f, 0.5f, 0.6f, 0.6f};

        p.filter = {FilterType::LowPass, 4000.0f, 1.5f, 0.4f};

        p.lfo = {Waveform::Triangle, 0.3f, 0.15f, true, false, false};

        p.macros[0].name = "Shimmer";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 12000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::OscDetune, 0.01f, 0.08f, MacroCurve::Linear});

        p.macros[1].name = "Width";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.0f, 0.15f, MacroCurve::Linear});

        p.macros[2].name = "Slow";
        p.macros[2].targets.add({MacroParamType::EnvAttack, 0.05f, 1.5f, MacroCurve::Exponential});
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.2f, 3.0f, MacroCurve::Exponential});
    }

    // Preset 3: "Dark Pad"
    {
        VoicePreset &p = presets[2];
        p.name = "Pad: Dark";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Square, 1.0f, 0.5f, 0.0f, 4, 0.04f};
        p.oscillators[1] = {Waveform::Sine, 0.5f, 0.5f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.5f, 0.6f, 0.7f, 1.5f};
        p.filterEnvelope = {0.3f, 1.0f, 0.3f, 1.0f};

        p.filter = {FilterType::LowPass, 800.0f, 1.2f, 0.2f};

        p.lfo = {Waveform::Sine, 0.2f, 0.1f, true, false, false};

        p.macros[0].name = "Darkness";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 200.0f, 2000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Depth";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.2f, 0.8f, MacroCurve::Linear});
        p.macros[1].targets.add({MacroParamType::FilterResonance, 0.5f, 3.0f, MacroCurve::Linear});

        p.macros[2].name = "Evolve";
        p.macros[2].targets.add({MacroParamType::LFORate, 0.05f, 0.5f, MacroCurve::Logarithmic});
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.4f, MacroCurve::Linear});
    }

    // ============= KEYS (4-7) =============

    // Preset 4: "Electric Piano"
    {
        VoicePreset &p = presets[3];
        p.name = "Keys: E.Piano";
        p.numOscillators = 3;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.6f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 2.0f, 0.3f, 0.0f, 1, 0.0f};
        p.oscillators[2] = {Waveform::Sine, 4.0f, 0.15f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 1.5f, 0.3f, 0.8f};
        p.filterEnvelope = {0.001f, 0.5f, 0.4f, 0.5f};

        p.filter = {FilterType::LowPass, 4000.0f, 1.0f, 0.4f};

        p.lfo = {Waveform::Sine, 5.0f, 0.0f, false, false, true};

        p.macros[0].name = "Bell";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 8000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::EnvAttack, 0.001f, 0.05f, MacroCurve::Linear});

        p.macros[1].name = "Body";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 0.5f, 2.5f, MacroCurve::Linear});
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.3f, 0.8f, MacroCurve::Linear});

        p.macros[2].name = "Tremolo";
        p.macros[2].targets.add({MacroParamType::LFORate, 1.0f, 10.0f, MacroCurve::Exponential});
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.5f, MacroCurve::Linear});
    }

    // Preset 5: "Organ"
    {
        VoicePreset &p = presets[4];
        p.name = "Keys: Organ";
        p.numOscillators = 4;

        p.oscillators[0] = {Waveform::Sine, 0.5f, 0.5f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 1.0f, 0.7f, 0.0f, 1, 0.0f};
        p.oscillators[2] = {Waveform::Sine, 2.0f, 0.4f, 0.0f, 1, 0.0f};
        p.oscillators[3] = {Waveform::Sine, 4.0f, 0.2f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.01f, 0.05f, 1.0f, 0.1f};
        p.filterEnvelope = {0.01f, 0.1f, 1.0f, 0.1f};

        p.filter = {FilterType::LowPass, 5000.0f, 0.7f, 0.0f};

        p.lfo = {Waveform::Sine, 6.0f, 0.0f, false, true, false};

        p.macros[0].name = "Drawbars";
        p.macros[0].targets.add({MacroParamType::OscLevel, 0.3f, 1.0f, MacroCurve::Linear});

        p.macros[1].name = "Percussion";
        p.macros[1].targets.add({MacroParamType::EnvAttack, 0.001f, 0.1f, MacroCurve::Linear});
        p.macros[1].targets.add({MacroParamType::FilterCutoff, 3000.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[2].name = "Leslie";
        p.macros[2].targets.add({MacroParamType::LFORate, 1.0f, 8.0f, MacroCurve::Exponential});
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.3f, MacroCurve::Linear});
    }

    // Preset 6: "Clav"
    {
        VoicePreset &p = presets[5];
        p.name = "Keys: Clav";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Square, 1.0f, 0.7f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Square, 2.0f, 0.3f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.3f, 0.4f, 0.2f};
        p.filterEnvelope = {0.001f, 0.2f, 0.3f, 0.15f};

        p.filter = {FilterType::BandPass, 1500.0f, 3.0f, 0.6f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Bite";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 800.0f, 4000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::FilterResonance, 1.0f, 5.0f, MacroCurve::Linear});

        p.macros[1].name = "Funk";
        p.macros[1].targets.add({MacroParamType::EnvAttack, 0.0001f, 0.01f, MacroCurve::Exponential});

        p.macros[2].name = "Decay";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.05f, 0.5f, MacroCurve::Exponential});
    }

    // Preset 7: "Bells"
    {
        VoicePreset &p = presets[6];
        p.name = "Bell: FM";
        p.numOscillators = 4;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.5f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 2.1f, 0.35f, 0.0f, 1, 0.0f};
        p.oscillators[2] = {Waveform::Sine, 3.05f, 0.2f, 0.0f, 1, 0.0f};
        p.oscillators[3] = {Waveform::Sine, 4.2f, 0.1f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 2.0f, 0.0f, 3.0f};
        p.filterEnvelope = {0.001f, 1.0f, 0.3f, 2.0f};

        p.filter = {FilterType::LowPass, 8000.0f, 0.7f, 0.2f};

        p.lfo = {Waveform::Sine, 4.0f, 0.0f, false, true, false};

        p.macros[0].name = "Brightness";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 4000.0f, 16000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Shimmer";
        p.macros[1].targets.add({MacroParamType::LFORate, 2.0f, 8.0f, MacroCurve::Linear});
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.0f, 0.2f, MacroCurve::Linear});

        p.macros[2].name = "Sustain";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 1.0f, 8.0f, MacroCurve::Exponential});
    }

    // ============= STRINGS (8-10) =============

    // Preset 8: "Strings"
    {
        VoicePreset &p = presets[7];
        p.name = "Strings: Ensemble";
        p.numOscillators = 3;

        // Use triangle + sine blend for warmer string tone, minimal detune
        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 4, 0.008f};
        p.oscillators[1] = {Waveform::Saw, 1.0f, 0.3f, 0.25f, 4, 0.006f};
        p.oscillators[2] = {Waveform::Triangle, 0.5f, 0.25f, 0.0f, 2, 0.004f};

        p.ampEnvelope = {0.25f, 0.2f, 0.85f, 0.5f};
        p.filterEnvelope = {0.15f, 0.3f, 0.6f, 0.4f};

        p.filter = {FilterType::LowPass, 2500.0f, 0.8f, 0.2f};

        p.lfo = {Waveform::Sine, 5.5f, 0.08f, false, true, false};  // Subtle vibrato on pitch

        p.macros[0].name = "Brightness";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Attack";
        p.macros[1].targets.add({MacroParamType::EnvAttack, 0.05f, 0.8f, MacroCurve::Exponential});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.2f, MacroCurve::Linear});
    }

    // Preset 9: "Pizzicato"
    {
        VoicePreset &p = presets[8];
        p.name = "Strings: Pizzicato";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Triangle, 1.0f, 0.7f, 0.0f, 2, 0.01f};
        p.oscillators[1] = {Waveform::Sine, 2.0f, 0.3f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.15f, 0.0f, 0.2f};
        p.filterEnvelope = {0.001f, 0.1f, 0.2f, 0.15f};

        p.filter = {FilterType::LowPass, 3500.0f, 1.5f, 0.5f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Attack";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Body";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.4f, 0.9f, MacroCurve::Linear});

        p.macros[2].name = "Decay";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.1f, 0.5f, MacroCurve::Linear});
    }

    // Preset 10: "Bow"
    {
        VoicePreset &p = presets[9];
        p.name = "Strings: Bowed";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.7f, 0.0f, 4, 0.02f};
        p.oscillators[1] = {Waveform::Noise, 1.0f, 0.1f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.8f, 0.5f, 0.85f, 0.8f};
        p.filterEnvelope = {0.5f, 0.8f, 0.6f, 0.6f};

        p.filter = {FilterType::LowPass, 2500.0f, 1.2f, 0.4f};

        p.lfo = {Waveform::Sine, 0.3f, 0.1f, true, false, false};

        p.macros[0].name = "Pressure";
        p.macros[0].targets.add({MacroParamType::OscLevel, 0.4f, 1.0f, MacroCurve::Linear});
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 4000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Rosin";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 0.5f, 2.5f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFORate, 4.0f, 7.0f, MacroCurve::Linear});
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.15f, MacroCurve::Linear});
    }

    // ============= SYNTH (11-13) =============

    // Preset 11: "Lead"
    {
        VoicePreset &p = presets[10];
        p.name = "Lead: Classic";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.7f, 0.0f, 2, 0.01f};
        p.oscillators[1] = {Waveform::Square, 1.0f, 0.4f, 0.25f, 1, 0.0f};

        p.ampEnvelope = {0.01f, 0.1f, 0.8f, 0.2f};
        p.filterEnvelope = {0.01f, 0.3f, 0.5f, 0.2f};

        p.filter = {FilterType::LowPass, 4000.0f, 2.5f, 0.5f};

        p.lfo = {Waveform::Sine, 5.0f, 0.0f, false, true, false};

        p.macros[0].name = "Bite";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 12000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::FilterResonance, 1.0f, 4.0f, MacroCurve::Linear});

        p.macros[1].name = "Drive";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.5f, 1.0f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFORate, 3.0f, 8.0f, MacroCurve::Linear});
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.3f, MacroCurve::Linear});
    }

    // Preset 12: "Bass"
    {
        VoicePreset &p = presets[11];
        p.name = "Bass: Synth";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Square, 1.0f, 0.7f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Saw, 0.5f, 0.5f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.01f, 0.2f, 0.7f, 0.15f};
        p.filterEnvelope = {0.01f, 0.3f, 0.3f, 0.2f};

        p.filter = {FilterType::LowPass, 1200.0f, 2.0f, 0.6f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Sub";
        p.macros[0].targets.add({MacroParamType::OscLevel, 0.3f, 0.8f, MacroCurve::Linear});

        p.macros[1].name = "Growl";
        p.macros[1].targets.add({MacroParamType::FilterCutoff, 500.0f, 3000.0f, MacroCurve::Exponential});
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.0f, 4.0f, MacroCurve::Linear});

        p.macros[2].name = "Punch";
        p.macros[2].targets.add({MacroParamType::EnvAttack, 0.001f, 0.05f, MacroCurve::Exponential});
    }

    // Preset 13: "Pluck"
    {
        VoicePreset &p = presets[12];
        p.name = "Synth: Pluck";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 3, 0.02f};
        p.oscillators[1] = {Waveform::Square, 2.0f, 0.3f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.4f, 0.0f, 0.3f};
        p.filterEnvelope = {0.001f, 0.25f, 0.1f, 0.2f};

        p.filter = {FilterType::LowPass, 5000.0f, 2.0f, 0.7f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Bright";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 3000.0f, 10000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Snap";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.0f, 4.0f, MacroCurve::Linear});

        p.macros[2].name = "Length";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.1f, 1.0f, MacroCurve::Exponential});
    }

    // ============= FX (14-16) =============

    // Preset 14: "Choir"
    {
        VoicePreset &p = presets[13];
        p.name = "Vocal: Choir";
        p.numOscillators = 3;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 6, 0.05f};
        p.oscillators[1] = {Waveform::Saw, 2.0f, 0.3f, 0.0f, 4, 0.03f};
        p.oscillators[2] = {Waveform::Sine, 3.0f, 0.15f, 0.0f, 2, 0.02f};

        p.ampEnvelope = {0.01f, 0.3f, 0.85f, 0.8f};
        p.filterEnvelope = {0.01f, 0.4f, 0.7f, 0.6f};

        p.filter = {FilterType::LowPass, 2500.0f, 1.2f, 0.3f};

        p.lfo = {Waveform::Sine, 0.3f, 0.2f, true, false, false};

        p.macros[0].name = "Vowel";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 800.0f, 3000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::FilterResonance, 1.0f, 3.0f, MacroCurve::Linear});

        p.macros[1].name = "Breathe";
        p.macros[1].targets.add({MacroParamType::EnvAttack, 0.2f, 1.0f, MacroCurve::Exponential});

        p.macros[2].name = "Ensemble";
        p.macros[2].targets.add({MacroParamType::OscDetune, 0.02f, 0.1f, MacroCurve::Linear});
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.1f, 0.4f, MacroCurve::Linear});
    }

    // Preset 15: "Noise"
    {
        VoicePreset &p = presets[14];
        p.name = "FX: Noise";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Noise, 1.0f, 0.6f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 1.0f, 0.4f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.1f, 0.5f, 0.6f, 0.5f};
        p.filterEnvelope = {0.05f, 0.4f, 0.4f, 0.4f};

        p.filter = {FilterType::LowPass, 3000.0f, 2.0f, 0.5f};

        p.lfo = {Waveform::Sine, 0.5f, 0.3f, true, false, false};

        p.macros[0].name = "Texture";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 500.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Resonance";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 0.5f, 5.0f, MacroCurve::Linear});

        p.macros[2].name = "Movement";
        p.macros[2].targets.add({MacroParamType::LFORate, 0.1f, 2.0f, MacroCurve::Exponential});
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.1f, 0.6f, MacroCurve::Linear});
    }

    // Preset 16: "Glass"
    {
        VoicePreset &p = presets[15];
        p.name = "Bell: Glass";
        p.numOscillators = 4;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.4f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 2.5f, 0.3f, 0.0f, 1, 0.0f};
        p.oscillators[2] = {Waveform::Sine, 4.1f, 0.2f, 0.0f, 1, 0.0f};
        p.oscillators[3] = {Waveform::Sine, 6.3f, 0.1f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.01f, 0.5f, 0.7f, 1.5f};
        p.filterEnvelope = {0.01f, 0.4f, 0.5f, 1.0f};

        p.filter = {FilterType::LowPass, 4000.0f, 0.7f, 0.3f};

        p.lfo = {Waveform::Sine, 3.0f, 0.0f, false, true, false};

        p.macros[0].name = "Crystal";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 500.0f, 4000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Sparkle";
        p.macros[1].targets.add({MacroParamType::LFORate, 2.0f, 6.0f, MacroCurve::Linear});
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.0f, 0.2f, MacroCurve::Linear});

        p.macros[2].name = "Ring";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 2.0f, 10.0f, MacroCurve::Exponential});
    }

    // ============= MORE STRINGS (17-24) =============

    // Preset 17: "Slow Strings"
    {
        VoicePreset &p = presets[16];
        p.name = "Strings: Slow";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.55f, 0.0f, 4, 0.006f};
        p.oscillators[1] = {Waveform::Saw, 0.5f, 0.35f, 0.0f, 3, 0.004f};

        p.ampEnvelope = {0.8f, 0.4f, 0.9f, 1.2f};
        p.filterEnvelope = {0.6f, 0.5f, 0.6f, 0.8f};

        p.filter = {FilterType::LowPass, 2000.0f, 0.7f, 0.25f};

        p.lfo = {Waveform::Sine, 5.0f, 0.06f, false, true, false};

        p.macros[0].name = "Warmth";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1000.0f, 4000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Swell";
        p.macros[1].targets.add({MacroParamType::EnvAttack, 0.5f, 2.0f, MacroCurve::Exponential});

        p.macros[2].name = "Depth";
        p.macros[2].targets.add({MacroParamType::OscLevel, 0.4f, 0.9f, MacroCurve::Linear});
    }

    // Preset 18: "Cello"
    {
        VoicePreset &p = presets[17];
        p.name = "Strings: Cello";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 3, 0.005f};
        p.oscillators[1] = {Waveform::Triangle, 0.5f, 0.3f, 0.0f, 2, 0.003f};

        p.ampEnvelope = {0.15f, 0.2f, 0.85f, 0.4f};
        p.filterEnvelope = {0.1f, 0.3f, 0.5f, 0.3f};

        p.filter = {FilterType::LowPass, 1800.0f, 0.9f, 0.2f};

        p.lfo = {Waveform::Sine, 5.5f, 0.1f, false, true, false};

        p.macros[0].name = "Bow Press";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1200.0f, 3500.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::OscLevel, 0.5f, 0.9f, MacroCurve::Linear});

        p.macros[1].name = "Vibrato";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.0f, 0.2f, MacroCurve::Linear});

        p.macros[2].name = "Release";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.2f, 1.5f, MacroCurve::Exponential});
    }

    // Preset 19: "Violin"
    {
        VoicePreset &p = presets[18];
        p.name = "Strings: Violin";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 2, 0.004f};
        p.oscillators[1] = {Waveform::Saw, 2.0f, 0.2f, 0.0f, 2, 0.003f};

        p.ampEnvelope = {0.1f, 0.15f, 0.8f, 0.35f};
        p.filterEnvelope = {0.08f, 0.2f, 0.6f, 0.25f};

        p.filter = {FilterType::LowPass, 3500.0f, 1.0f, 0.25f};

        p.lfo = {Waveform::Sine, 6.0f, 0.12f, false, true, false};

        p.macros[0].name = "Brightness";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2500.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Vibrato";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.0f, 0.25f, MacroCurve::Linear});

        p.macros[2].name = "Expression";
        p.macros[2].targets.add({MacroParamType::OscLevel, 0.4f, 1.0f, MacroCurve::Linear});
    }

    // Preset 20: "Viola"
    {
        VoicePreset &p = presets[19];
        p.name = "Strings: Viola";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.55f, 0.0f, 3, 0.005f};
        p.oscillators[1] = {Waveform::Triangle, 1.0f, 0.3f, 0.0f, 2, 0.004f};

        p.ampEnvelope = {0.12f, 0.18f, 0.82f, 0.38f};
        p.filterEnvelope = {0.09f, 0.25f, 0.55f, 0.28f};

        p.filter = {FilterType::LowPass, 2800.0f, 0.9f, 0.22f};

        p.lfo = {Waveform::Sine, 5.5f, 0.1f, false, true, false};

        p.macros[0].name = "Warmth";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1800.0f, 4500.0f, MacroCurve::Exponential});

        p.macros[1].name = "Vibrato";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.0f, 0.22f, MacroCurve::Linear});

        p.macros[2].name = "Dynamics";
        p.macros[2].targets.add({MacroParamType::OscLevel, 0.45f, 0.95f, MacroCurve::Linear});
    }

    // Preset 21: "Contrabass"
    {
        VoicePreset &p = presets[20];
        p.name = "Strings: Bass";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.65f, 0.0f, 3, 0.004f};
        p.oscillators[1] = {Waveform::Sine, 0.5f, 0.4f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.2f, 0.25f, 0.88f, 0.5f};
        p.filterEnvelope = {0.15f, 0.35f, 0.5f, 0.4f};

        p.filter = {FilterType::LowPass, 1200.0f, 0.8f, 0.2f};

        p.lfo = {Waveform::Sine, 4.5f, 0.06f, false, true, false};

        p.macros[0].name = "Body";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 800.0f, 2500.0f, MacroCurve::Exponential});

        p.macros[1].name = "Bow";
        p.macros[1].targets.add({MacroParamType::EnvAttack, 0.1f, 0.6f, MacroCurve::Exponential});

        p.macros[2].name = "Depth";
        p.macros[2].targets.add({MacroParamType::OscLevel, 0.5f, 1.0f, MacroCurve::Linear});
    }

    // Preset 22: "Chamber"
    {
        VoicePreset &p = presets[21];
        p.name = "Strings: Chamber";
        p.numOscillators = 3;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.4f, 0.0f, 3, 0.005f};
        p.oscillators[1] = {Waveform::Saw, 2.0f, 0.25f, 0.0f, 2, 0.004f};
        p.oscillators[2] = {Waveform::Triangle, 0.5f, 0.2f, 0.0f, 2, 0.003f};

        p.ampEnvelope = {0.18f, 0.22f, 0.85f, 0.45f};
        p.filterEnvelope = {0.12f, 0.28f, 0.6f, 0.35f};

        p.filter = {FilterType::LowPass, 2800.0f, 0.85f, 0.22f};

        p.lfo = {Waveform::Sine, 5.2f, 0.08f, false, true, false};

        p.macros[0].name = "Fullness";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 5000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::OscLevel, 0.5f, 0.9f, MacroCurve::Linear});

        p.macros[1].name = "Ensemble";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.0f, 0.01f, MacroCurve::Linear});

        p.macros[2].name = "Expression";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.15f, MacroCurve::Linear});
    }

    // Preset 23: "Sweep Pad"
    {
        VoicePreset &p = presets[22];
        p.name = "Pad: Sweep";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 4, 0.02f};
        p.oscillators[1] = {Waveform::Saw, 2.0f, 0.35f, 0.0f, 3, 0.015f};

        p.ampEnvelope = {0.4f, 0.5f, 0.85f, 1.0f};
        p.filterEnvelope = {0.5f, 1.5f, 0.3f, 0.8f};

        p.filter = {FilterType::LowPass, 800.0f, 2.5f, 0.7f};

        p.lfo = {Waveform::Sine, 0.15f, 0.4f, true, false, false};

        p.macros[0].name = "Sweep";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 400.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Resonance";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.0f, 4.0f, MacroCurve::Linear});

        p.macros[2].name = "Motion";
        p.macros[2].targets.add({MacroParamType::LFORate, 0.05f, 0.5f, MacroCurve::Exponential});
    }

    // Preset 24: "Analog Pad"
    {
        VoicePreset &p = presets[23];
        p.name = "Pad: Analog";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 3, 0.015f};
        p.oscillators[1] = {Waveform::Square, 0.5f, 0.4f, 0.0f, 2, 0.01f};

        p.ampEnvelope = {0.35f, 0.4f, 0.8f, 0.9f};
        p.filterEnvelope = {0.25f, 0.6f, 0.5f, 0.7f};

        p.filter = {FilterType::LowPass, 1800.0f, 1.8f, 0.35f};

        p.lfo = {Waveform::Triangle, 0.25f, 0.2f, true, false, false};

        p.macros[0].name = "Cutoff";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 600.0f, 5000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Vintage";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.005f, 0.03f, MacroCurve::Linear});

        p.macros[2].name = "Drift";
        p.macros[2].targets.add({MacroParamType::LFORate, 0.1f, 0.6f, MacroCurve::Linear});
    }

    // ============= BRASS (25-32) =============

    // Preset 25: "Brass"
    {
        VoicePreset &p = presets[24];
        p.name = "Brass: Section";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 3, 0.008f};
        p.oscillators[1] = {Waveform::Square, 1.0f, 0.3f, 0.0f, 2, 0.005f};

        p.ampEnvelope = {0.08f, 0.15f, 0.85f, 0.25f};
        p.filterEnvelope = {0.05f, 0.2f, 0.6f, 0.2f};

        p.filter = {FilterType::LowPass, 2000.0f, 1.5f, 0.5f};

        p.lfo = {Waveform::Sine, 5.0f, 0.0f, false, true, false};

        p.macros[0].name = "Blare";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Attack";
        p.macros[1].targets.add({MacroParamType::EnvAttack, 0.01f, 0.2f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.15f, MacroCurve::Linear});
    }

    // Preset 26: "Trumpet"
    {
        VoicePreset &p = presets[25];
        p.name = "Brass: Trumpet";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.65f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Square, 2.0f, 0.2f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.03f, 0.1f, 0.8f, 0.2f};
        p.filterEnvelope = {0.02f, 0.15f, 0.65f, 0.15f};

        p.filter = {FilterType::LowPass, 3000.0f, 1.8f, 0.55f};

        p.lfo = {Waveform::Sine, 5.5f, 0.0f, false, true, false};

        p.macros[0].name = "Bright";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Mute";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.0f, 4.0f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.2f, MacroCurve::Linear});
    }

    // Preset 27: "Trombone"
    {
        VoicePreset &p = presets[26];
        p.name = "Brass: Trombone";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.65f, 0.0f, 2, 0.003f};
        p.oscillators[1] = {Waveform::Square, 0.5f, 0.25f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.06f, 0.12f, 0.82f, 0.22f};
        p.filterEnvelope = {0.04f, 0.18f, 0.55f, 0.18f};

        p.filter = {FilterType::LowPass, 1800.0f, 1.5f, 0.45f};

        p.lfo = {Waveform::Sine, 5.0f, 0.0f, false, true, false};

        p.macros[0].name = "Slide";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1200.0f, 5000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Body";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.5f, 1.0f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.18f, MacroCurve::Linear});
    }

    // Preset 28: "French Horn"
    {
        VoicePreset &p = presets[27];
        p.name = "Brass: French Horn";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 2, 0.004f};
        p.oscillators[1] = {Waveform::Sine, 1.0f, 0.35f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.12f, 0.2f, 0.8f, 0.3f};
        p.filterEnvelope = {0.1f, 0.25f, 0.5f, 0.25f};

        p.filter = {FilterType::LowPass, 1500.0f, 1.2f, 0.35f};

        p.lfo = {Waveform::Sine, 4.5f, 0.0f, false, true, false};

        p.macros[0].name = "Open";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1000.0f, 4000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Mellow";
        p.macros[1].targets.add({MacroParamType::EnvAttack, 0.05f, 0.3f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.15f, MacroCurve::Linear});
    }

    // Preset 29: "Sax Alto"
    {
        VoicePreset &p = presets[28];
        p.name = "Wind: Alto Sax";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Square, 1.0f, 0.5f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Saw, 2.0f, 0.3f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.04f, 0.12f, 0.75f, 0.2f};
        p.filterEnvelope = {0.03f, 0.15f, 0.55f, 0.15f};

        p.filter = {FilterType::LowPass, 2500.0f, 2.0f, 0.5f};

        p.lfo = {Waveform::Sine, 5.5f, 0.1f, false, true, false};

        p.macros[0].name = "Breath";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 6000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::OscLevel, 0.5f, 1.0f, MacroCurve::Linear});

        p.macros[1].name = "Reed";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.5f, 4.0f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.25f, MacroCurve::Linear});
    }

    // Preset 30: "Sax Tenor"
    {
        VoicePreset &p = presets[29];
        p.name = "Wind: Tenor Sax";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Square, 1.0f, 0.55f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Saw, 1.0f, 0.3f, 0.0f, 2, 0.003f};

        p.ampEnvelope = {0.05f, 0.15f, 0.78f, 0.22f};
        p.filterEnvelope = {0.04f, 0.18f, 0.5f, 0.18f};

        p.filter = {FilterType::LowPass, 2000.0f, 2.2f, 0.45f};

        p.lfo = {Waveform::Sine, 5.0f, 0.08f, false, true, false};

        p.macros[0].name = "Breath";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1200.0f, 5000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::OscLevel, 0.5f, 1.0f, MacroCurve::Linear});

        p.macros[1].name = "Growl";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.5f, 5.0f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.22f, MacroCurve::Linear});
    }

    // Preset 31: "Oboe"
    {
        VoicePreset &p = presets[30];
        p.name = "Wind: Oboe";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 2.0f, 0.35f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.06f, 0.1f, 0.8f, 0.18f};
        p.filterEnvelope = {0.04f, 0.12f, 0.6f, 0.12f};

        p.filter = {FilterType::BandPass, 2000.0f, 2.5f, 0.4f};

        p.lfo = {Waveform::Sine, 5.5f, 0.08f, false, true, false};

        p.macros[0].name = "Nasal";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 4000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Reed";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 2.0f, 5.0f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.18f, MacroCurve::Linear});
    }

    // Preset 32: "Clarinet"
    {
        VoicePreset &p = presets[31];
        p.name = "Wind: Clarinet";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Square, 1.0f, 0.6f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 3.0f, 0.2f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.05f, 0.1f, 0.82f, 0.15f};
        p.filterEnvelope = {0.03f, 0.12f, 0.55f, 0.12f};

        p.filter = {FilterType::LowPass, 2200.0f, 1.8f, 0.4f};

        p.lfo = {Waveform::Sine, 5.2f, 0.06f, false, true, false};

        p.macros[0].name = "Register";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 5000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Tone";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.5f, 1.0f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.15f, MacroCurve::Linear});
    }

    // ============= SYNTH LEADS (33-40) =============

    // Preset 33: "Square Lead"
    {
        VoicePreset &p = presets[32];
        p.name = "Lead: Square";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Square, 1.0f, 0.6f, 0.0f, 2, 0.008f};
        p.oscillators[1] = {Waveform::Square, 2.0f, 0.25f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.01f, 0.1f, 0.8f, 0.15f};
        p.filterEnvelope = {0.01f, 0.15f, 0.5f, 0.1f};

        p.filter = {FilterType::LowPass, 3500.0f, 1.5f, 0.4f};

        p.lfo = {Waveform::Sine, 5.5f, 0.0f, false, true, false};

        p.macros[0].name = "Cutoff";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "PWM";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.0f, 0.02f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.2f, MacroCurve::Linear});
    }

    // Preset 34: "Saw Lead"
    {
        VoicePreset &p = presets[33];
        p.name = "Lead: Saw";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.65f, 0.0f, 2, 0.01f};
        p.oscillators[1] = {Waveform::Saw, 1.0f, 0.35f, 0.0f, 2, 0.008f};

        p.ampEnvelope = {0.01f, 0.08f, 0.85f, 0.12f};
        p.filterEnvelope = {0.01f, 0.12f, 0.55f, 0.1f};

        p.filter = {FilterType::LowPass, 4000.0f, 1.3f, 0.5f};

        p.lfo = {Waveform::Sine, 5.8f, 0.0f, false, true, false};

        p.macros[0].name = "Bright";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 10000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Fatness";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.005f, 0.03f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.22f, MacroCurve::Linear});
    }

    // Preset 35: "Sync Lead"
    {
        VoicePreset &p = presets[34];
        p.name = "Lead: Sync";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Saw, 2.0f, 0.55f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.005f, 0.1f, 0.75f, 0.1f};
        p.filterEnvelope = {0.01f, 0.2f, 0.4f, 0.15f};

        p.filter = {FilterType::LowPass, 5000.0f, 1.8f, 0.6f};

        p.lfo = {Waveform::Sine, 6.0f, 0.0f, false, true, false};

        p.macros[0].name = "Sync";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 12000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Resonance";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.0f, 4.0f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.25f, MacroCurve::Linear});
    }

    // Preset 36: "Poly Lead"
    {
        VoicePreset &p = presets[35];
        p.name = "Lead: Poly";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 4, 0.015f};
        p.oscillators[1] = {Waveform::Square, 2.0f, 0.3f, 0.0f, 2, 0.01f};

        p.ampEnvelope = {0.02f, 0.15f, 0.75f, 0.2f};
        p.filterEnvelope = {0.01f, 0.2f, 0.5f, 0.15f};

        p.filter = {FilterType::LowPass, 3500.0f, 1.5f, 0.45f};

        p.lfo = {Waveform::Sine, 5.5f, 0.0f, false, true, false};

        p.macros[0].name = "Shine";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Width";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.01f, 0.04f, MacroCurve::Linear});

        p.macros[2].name = "Vibrato";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.0f, 0.2f, MacroCurve::Linear});
    }

    // Preset 37: "Acid"
    {
        VoicePreset &p = presets[36];
        p.name = "Lead: Acid";
        p.numOscillators = 1;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.8f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.1f, 0.6f, 0.15f};
        p.filterEnvelope = {0.001f, 0.15f, 0.2f, 0.1f};

        p.filter = {FilterType::LowPass, 1500.0f, 4.0f, 0.8f};

        p.lfo = {Waveform::Saw, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Cutoff";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 500.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Reso";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 2.0f, 6.0f, MacroCurve::Linear});

        p.macros[2].name = "Decay";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.05f, 0.5f, MacroCurve::Exponential});
    }

    // Preset 38: "Hoover"
    {
        VoicePreset &p = presets[37];
        p.name = "Lead: Hoover";
        p.numOscillators = 3;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 3, 0.025f};
        p.oscillators[1] = {Waveform::Saw, 0.5f, 0.4f, 0.0f, 3, 0.02f};
        p.oscillators[2] = {Waveform::Square, 2.0f, 0.25f, 0.0f, 2, 0.015f};

        p.ampEnvelope = {0.01f, 0.15f, 0.8f, 0.2f};
        p.filterEnvelope = {0.01f, 0.2f, 0.5f, 0.15f};

        p.filter = {FilterType::LowPass, 3000.0f, 1.5f, 0.5f};

        p.lfo = {Waveform::Sine, 6.0f, 0.1f, false, true, false};

        p.macros[0].name = "Scream";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Chaos";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.015f, 0.06f, MacroCurve::Linear});

        p.macros[2].name = "Grind";
        p.macros[2].targets.add({MacroParamType::FilterResonance, 1.0f, 4.0f, MacroCurve::Linear});
    }

    // Preset 39: "Trance Lead"
    {
        VoicePreset &p = presets[38];
        p.name = "Lead: Trance";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 5, 0.02f};
        p.oscillators[1] = {Waveform::Saw, 2.0f, 0.3f, 0.0f, 3, 0.015f};

        p.ampEnvelope = {0.01f, 0.1f, 0.8f, 0.15f};
        p.filterEnvelope = {0.005f, 0.15f, 0.6f, 0.1f};

        p.filter = {FilterType::LowPass, 4500.0f, 1.8f, 0.55f};

        p.lfo = {Waveform::Sine, 6.0f, 0.08f, false, true, false};

        p.macros[0].name = "Gate";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2500.0f, 10000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Super";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.01f, 0.05f, MacroCurve::Linear});

        p.macros[2].name = "Pump";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.1f, 0.4f, MacroCurve::Linear});
    }

    // Preset 40: "Wobble"
    {
        VoicePreset &p = presets[39];
        p.name = "Lead: Wobble";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.65f, 0.0f, 2, 0.01f};
        p.oscillators[1] = {Waveform::Square, 0.5f, 0.4f, 0.0f, 2, 0.008f};

        p.ampEnvelope = {0.01f, 0.1f, 0.9f, 0.2f};
        p.filterEnvelope = {0.01f, 0.05f, 0.8f, 0.1f};

        p.filter = {FilterType::LowPass, 2000.0f, 3.0f, 0.6f};

        p.lfo = {Waveform::Sine, 4.0f, 0.6f, true, false, false};

        p.macros[0].name = "Rate";
        p.macros[0].targets.add({MacroParamType::LFORate, 1.0f, 12.0f, MacroCurve::Exponential});

        p.macros[1].name = "Depth";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.3f, 0.9f, MacroCurve::Linear});

        p.macros[2].name = "Reso";
        p.macros[2].targets.add({MacroParamType::FilterResonance, 2.0f, 5.0f, MacroCurve::Linear});
    }

    // ============= SYNTH BASS (41-48) =============

    // Preset 41: "Analog Bass"
    {
        VoicePreset &p = presets[40];
        p.name = "Bass: Analog";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 2, 0.008f};
        p.oscillators[1] = {Waveform::Square, 0.5f, 0.4f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.005f, 0.15f, 0.7f, 0.2f};
        p.filterEnvelope = {0.001f, 0.2f, 0.3f, 0.15f};

        p.filter = {FilterType::LowPass, 1500.0f, 2.0f, 0.6f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Growl";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 600.0f, 4000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Punch";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.5f, 4.5f, MacroCurve::Linear});

        p.macros[2].name = "Decay";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.1f, 0.6f, MacroCurve::Exponential});
    }

    // Preset 42: "Sub Bass"
    {
        VoicePreset &p = presets[41];
        p.name = "Bass: Sub";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.8f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 0.5f, 0.3f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.002f, 0.08f, 0.9f, 0.25f};
        p.filterEnvelope = {0.001f, 0.1f, 0.8f, 0.15f};

        p.filter = {FilterType::LowPass, 800.0f, 1.0f, 0.2f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Thump";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 400.0f, 1500.0f, MacroCurve::Exponential});

        p.macros[1].name = "Weight";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.6f, 1.0f, MacroCurve::Linear});

        p.macros[2].name = "Release";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.15f, 0.8f, MacroCurve::Exponential});
    }

    // Preset 43: "Reese"
    {
        VoicePreset &p = presets[42];
        p.name = "Bass: Reese";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.55f, 0.0f, 3, 0.015f};
        p.oscillators[1] = {Waveform::Saw, 0.5f, 0.45f, 0.0f, 3, 0.012f};

        p.ampEnvelope = {0.01f, 0.1f, 0.85f, 0.3f};
        p.filterEnvelope = {0.01f, 0.15f, 0.6f, 0.2f};

        p.filter = {FilterType::LowPass, 1200.0f, 1.8f, 0.4f};

        p.lfo = {Waveform::Sine, 0.2f, 0.15f, true, false, false};

        p.macros[0].name = "Grit";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 600.0f, 3500.0f, MacroCurve::Exponential});

        p.macros[1].name = "Phase";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.01f, 0.04f, MacroCurve::Linear});

        p.macros[2].name = "Motion";
        p.macros[2].targets.add({MacroParamType::LFORate, 0.1f, 0.8f, MacroCurve::Linear});
    }

    // Preset 44: "Pluck Bass"
    {
        VoicePreset &p = presets[43];
        p.name = "Bass: Pluck";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Triangle, 1.0f, 0.65f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Saw, 0.5f, 0.3f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.2f, 0.4f, 0.2f};
        p.filterEnvelope = {0.001f, 0.15f, 0.2f, 0.1f};

        p.filter = {FilterType::LowPass, 2500.0f, 2.5f, 0.7f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Snap";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Reso";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.5f, 5.0f, MacroCurve::Linear});

        p.macros[2].name = "Sustain";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.1f, 0.6f, MacroCurve::Linear});
    }

    // Preset 45: "FM Bass"
    {
        VoicePreset &p = presets[44];
        p.name = "Bass: FM";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.7f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 2.0f, 0.45f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.12f, 0.6f, 0.15f};
        p.filterEnvelope = {0.001f, 0.1f, 0.4f, 0.1f};

        p.filter = {FilterType::LowPass, 3000.0f, 1.5f, 0.5f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Metallic";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1000.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Harmonic";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.3f, 0.8f, MacroCurve::Linear});

        p.macros[2].name = "Decay";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.1f, 0.5f, MacroCurve::Exponential});
    }

    // Preset 46: "Wobble Bass"
    {
        VoicePreset &p = presets[45];
        p.name = "Bass: Wobble";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 2, 0.01f};
        p.oscillators[1] = {Waveform::Square, 0.5f, 0.4f, 0.0f, 2, 0.008f};

        p.ampEnvelope = {0.01f, 0.1f, 0.85f, 0.25f};
        p.filterEnvelope = {0.01f, 0.05f, 0.7f, 0.15f};

        p.filter = {FilterType::LowPass, 1500.0f, 3.5f, 0.7f};

        p.lfo = {Waveform::Sine, 3.0f, 0.7f, true, false, false};

        p.macros[0].name = "Rate";
        p.macros[0].targets.add({MacroParamType::LFORate, 0.5f, 8.0f, MacroCurve::Exponential});

        p.macros[1].name = "Depth";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.4f, 0.95f, MacroCurve::Linear});

        p.macros[2].name = "Nasty";
        p.macros[2].targets.add({MacroParamType::FilterResonance, 2.5f, 6.0f, MacroCurve::Linear});
    }

    // Preset 47: "Growl Bass"
    {
        VoicePreset &p = presets[46];
        p.name = "Bass: Growl";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.55f, 0.0f, 3, 0.02f};
        p.oscillators[1] = {Waveform::Square, 0.5f, 0.45f, 0.0f, 2, 0.015f};

        p.ampEnvelope = {0.01f, 0.15f, 0.8f, 0.2f};
        p.filterEnvelope = {0.01f, 0.2f, 0.5f, 0.15f};

        p.filter = {FilterType::LowPass, 1200.0f, 3.0f, 0.65f};

        p.lfo = {Waveform::Triangle, 5.0f, 0.2f, true, false, false};

        p.macros[0].name = "Aggro";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 600.0f, 4000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Snarl";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 2.0f, 5.5f, MacroCurve::Linear});

        p.macros[2].name = "Motion";
        p.macros[2].targets.add({MacroParamType::LFORate, 2.0f, 10.0f, MacroCurve::Exponential});
    }

    // Preset 48: "808 Bass"
    {
        VoicePreset &p = presets[47];
        p.name = "Bass: 808";
        p.numOscillators = 1;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.9f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.3f, 0.6f, 0.8f};
        p.filterEnvelope = {0.001f, 0.4f, 0.3f, 0.5f};

        p.filter = {FilterType::LowPass, 600.0f, 1.2f, 0.4f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Boom";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 300.0f, 1200.0f, MacroCurve::Exponential});

        p.macros[1].name = "Sustain";
        p.macros[1].targets.add({MacroParamType::EnvRelease, 0.4f, 2.0f, MacroCurve::Exponential});

        p.macros[2].name = "Thump";
        p.macros[2].targets.add({MacroParamType::OscLevel, 0.7f, 1.0f, MacroCurve::Linear});
    }

    // ============= BELLS & MALLETS (49-56) =============

    // Preset 49: "Vibraphone"
    {
        VoicePreset &p = presets[48];
        p.name = "Bell: Vibes";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.65f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 4.0f, 0.25f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.05f, 0.3f, 1.2f};
        p.filterEnvelope = {0.001f, 0.08f, 0.4f, 0.8f};

        p.filter = {FilterType::LowPass, 4000.0f, 0.8f, 0.2f};

        p.lfo = {Waveform::Sine, 5.5f, 0.15f, false, false, true};

        p.macros[0].name = "Brightness";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2500.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Motor";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.0f, 0.3f, MacroCurve::Linear});

        p.macros[2].name = "Sustain";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.8f, 4.0f, MacroCurve::Exponential});
    }

    // Preset 50: "Marimba"
    {
        VoicePreset &p = presets[49];
        p.name = "Bell: Marimba";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.7f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 4.0f, 0.2f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.08f, 0.2f, 0.5f};
        p.filterEnvelope = {0.001f, 0.1f, 0.3f, 0.4f};

        p.filter = {FilterType::LowPass, 3500.0f, 0.9f, 0.25f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Woody";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2000.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Mallet";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.4f, 0.9f, MacroCurve::Linear});

        p.macros[2].name = "Decay";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.3f, 1.5f, MacroCurve::Exponential});
    }

    // Preset 51: "Xylophone"
    {
        VoicePreset &p = presets[50];
        p.name = "Bell: Xylophone";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.6f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 3.0f, 0.35f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.03f, 0.15f, 0.3f};
        p.filterEnvelope = {0.001f, 0.05f, 0.2f, 0.2f};

        p.filter = {FilterType::LowPass, 5000.0f, 1.0f, 0.3f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Bright";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 3000.0f, 10000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Ring";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.3f, 0.8f, MacroCurve::Linear});

        p.macros[2].name = "Sustain";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.15f, 0.8f, MacroCurve::Exponential});
    }

    // Preset 52: "Glockenspiel"
    {
        VoicePreset &p = presets[51];
        p.name = "Bell: Glock";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.55f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 2.5f, 0.4f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.02f, 0.2f, 1.5f};
        p.filterEnvelope = {0.001f, 0.03f, 0.3f, 1.0f};

        p.filter = {FilterType::HighPass, 2000.0f, 0.8f, 0.15f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Shimmer";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 5000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Sparkle";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.3f, 0.8f, MacroCurve::Linear});

        p.macros[2].name = "Ring";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 1.0f, 4.0f, MacroCurve::Exponential});
    }

    // Preset 53: "Tubular Bells"
    {
        VoicePreset &p = presets[52];
        p.name = "Bell: Tubular";
        p.numOscillators = 3;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.5f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 2.0f, 0.35f, 0.0f, 1, 0.0f};
        p.oscillators[2] = {Waveform::Sine, 3.5f, 0.2f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.04f, 0.25f, 2.5f};
        p.filterEnvelope = {0.001f, 0.06f, 0.35f, 1.5f};

        p.filter = {FilterType::LowPass, 4500.0f, 0.9f, 0.2f};

        p.lfo = {Waveform::Sine, 3.0f, 0.08f, false, false, true};

        p.macros[0].name = "Tone";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2500.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Chime";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.0f, 0.15f, MacroCurve::Linear});

        p.macros[2].name = "Sustain";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 1.5f, 6.0f, MacroCurve::Exponential});
    }

    // Preset 54: "Music Box"
    {
        VoicePreset &p = presets[53];
        p.name = "Bell: Music Box";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Triangle, 1.0f, 0.6f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 3.0f, 0.3f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.02f, 0.1f, 0.8f};
        p.filterEnvelope = {0.001f, 0.03f, 0.2f, 0.5f};

        p.filter = {FilterType::LowPass, 5500.0f, 0.9f, 0.2f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Delicate";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 3500.0f, 10000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Tinkle";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.4f, 0.9f, MacroCurve::Linear});

        p.macros[2].name = "Ring";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.5f, 2.0f, MacroCurve::Exponential});
    }

    // Preset 55: "Steel Drum"
    {
        VoicePreset &p = presets[54];
        p.name = "Bell: Steel Drum";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.6f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 2.3f, 0.4f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.06f, 0.25f, 0.6f};
        p.filterEnvelope = {0.001f, 0.08f, 0.35f, 0.4f};

        p.filter = {FilterType::BandPass, 2500.0f, 1.5f, 0.3f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Metallic";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 5000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Pan";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.0f, 3.0f, MacroCurve::Linear});

        p.macros[2].name = "Sustain";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.3f, 1.5f, MacroCurve::Exponential});
    }

    // Preset 56: "Kalimba"
    {
        VoicePreset &p = presets[55];
        p.name = "Bell: Kalimba";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.65f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Triangle, 3.0f, 0.25f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.04f, 0.2f, 0.7f};
        p.filterEnvelope = {0.001f, 0.05f, 0.3f, 0.5f};

        p.filter = {FilterType::LowPass, 4000.0f, 1.0f, 0.25f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Pluck";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 2500.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Tine";
        p.macros[1].targets.add({MacroParamType::OscLevel, 0.4f, 0.9f, MacroCurve::Linear});

        p.macros[2].name = "Ring";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.4f, 1.8f, MacroCurve::Exponential});
    }

    // ============= SPECIAL FX (57-64) =============

    // Preset 57: "Atmosphere"
    {
        VoicePreset &p = presets[56];
        p.name = "FX: Atmosphere";
        p.numOscillators = 3;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.4f, 0.0f, 5, 0.03f};
        p.oscillators[1] = {Waveform::Sine, 2.0f, 0.3f, 0.0f, 3, 0.02f};
        p.oscillators[2] = {Waveform::Triangle, 0.5f, 0.25f, 0.0f, 4, 0.025f};

        p.ampEnvelope = {1.0f, 1.0f, 0.9f, 2.0f};
        p.filterEnvelope = {0.8f, 1.5f, 0.5f, 1.5f};

        p.filter = {FilterType::LowPass, 1500.0f, 1.5f, 0.35f};

        p.lfo = {Waveform::Sine, 0.1f, 0.3f, true, false, false};

        p.macros[0].name = "Evolve";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 800.0f, 5000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Depth";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.1f, 0.6f, MacroCurve::Linear});

        p.macros[2].name = "Space";
        p.macros[2].targets.add({MacroParamType::OscDetune, 0.02f, 0.08f, MacroCurve::Linear});
    }

    // Preset 58: "Sci-Fi"
    {
        VoicePreset &p = presets[57];
        p.name = "FX: Sci-Fi";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 2, 0.02f};
        p.oscillators[1] = {Waveform::Square, 2.5f, 0.4f, 0.0f, 2, 0.015f};

        p.ampEnvelope = {0.5f, 0.8f, 0.75f, 1.2f};
        p.filterEnvelope = {0.3f, 1.0f, 0.4f, 0.8f};

        p.filter = {FilterType::LowPass, 2000.0f, 3.0f, 0.6f};

        p.lfo = {Waveform::Saw, 0.3f, 0.5f, true, false, false};

        p.macros[0].name = "Sweep";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 500.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Alien";
        p.macros[1].targets.add({MacroParamType::LFORate, 0.1f, 2.0f, MacroCurve::Exponential});

        p.macros[2].name = "Morph";
        p.macros[2].targets.add({MacroParamType::FilterResonance, 2.0f, 6.0f, MacroCurve::Linear});
    }

    // Preset 59: "Crystal"
    {
        VoicePreset &p = presets[58];
        p.name = "FX: Crystal";
        p.numOscillators = 3;

        p.oscillators[0] = {Waveform::Sine, 1.0f, 0.45f, 0.0f, 2, 0.01f};
        p.oscillators[1] = {Waveform::Sine, 2.0f, 0.35f, 0.0f, 2, 0.008f};
        p.oscillators[2] = {Waveform::Sine, 4.0f, 0.2f, 0.0f, 2, 0.006f};

        p.ampEnvelope = {0.02f, 0.1f, 0.4f, 2.5f};
        p.filterEnvelope = {0.01f, 0.15f, 0.5f, 1.5f};

        p.filter = {FilterType::HighPass, 1500.0f, 1.2f, 0.25f};

        p.lfo = {Waveform::Triangle, 3.0f, 0.1f, false, false, true};

        p.macros[0].name = "Sparkle";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1000.0f, 4000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Shimmer";
        p.macros[1].targets.add({MacroParamType::LFODepth, 0.0f, 0.25f, MacroCurve::Linear});

        p.macros[2].name = "Decay";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 1.5f, 5.0f, MacroCurve::Exponential});
    }

    // Preset 60: "Haunted"
    {
        VoicePreset &p = presets[59];
        p.name = "FX: Haunted";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Square, 1.0f, 0.4f, 0.0f, 4, 0.025f};
        p.oscillators[1] = {Waveform::Sine, 0.5f, 0.35f, 0.0f, 3, 0.02f};

        p.ampEnvelope = {0.8f, 1.0f, 0.7f, 2.5f};
        p.filterEnvelope = {0.6f, 1.5f, 0.4f, 2.0f};

        p.filter = {FilterType::LowPass, 1200.0f, 2.5f, 0.5f};

        p.lfo = {Waveform::Sine, 0.15f, 0.4f, true, false, false};

        p.macros[0].name = "Eerie";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 600.0f, 3500.0f, MacroCurve::Exponential});

        p.macros[1].name = "Ghost";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.015f, 0.05f, MacroCurve::Linear});

        p.macros[2].name = "Haunt";
        p.macros[2].targets.add({MacroParamType::LFORate, 0.05f, 0.4f, MacroCurve::Exponential});
    }

    // Preset 61: "Metallic"
    {
        VoicePreset &p = presets[60];
        p.name = "FX: Metallic";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.5f, 0.0f, 1, 0.0f};
        p.oscillators[1] = {Waveform::Sine, 2.7f, 0.45f, 0.0f, 1, 0.0f};

        p.ampEnvelope = {0.001f, 0.08f, 0.35f, 1.0f};
        p.filterEnvelope = {0.001f, 0.1f, 0.4f, 0.6f};

        p.filter = {FilterType::BandPass, 3000.0f, 3.0f, 0.4f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Ring";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1500.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Clang";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 2.0f, 6.0f, MacroCurve::Linear});

        p.macros[2].name = "Sustain";
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.5f, 3.0f, MacroCurve::Exponential});
    }

    // Preset 62: "Sweep Arp"
    {
        VoicePreset &p = presets[61];
        p.name = "FX: Sweep";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.6f, 0.0f, 3, 0.015f};
        p.oscillators[1] = {Waveform::Square, 2.0f, 0.3f, 0.0f, 2, 0.01f};

        p.ampEnvelope = {0.01f, 0.05f, 0.6f, 0.2f};
        p.filterEnvelope = {0.01f, 0.15f, 0.3f, 0.15f};

        p.filter = {FilterType::LowPass, 3000.0f, 2.5f, 0.65f};

        p.lfo = {Waveform::Saw, 0.08f, 0.5f, true, false, false};

        p.macros[0].name = "Sweep";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1000.0f, 8000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Speed";
        p.macros[1].targets.add({MacroParamType::LFORate, 0.02f, 0.3f, MacroCurve::Exponential});

        p.macros[2].name = "Reso";
        p.macros[2].targets.add({MacroParamType::FilterResonance, 1.5f, 5.0f, MacroCurve::Linear});
    }

    // Preset 63: "Distorted"
    {
        VoicePreset &p = presets[62];
        p.name = "FX: Distorted";
        p.numOscillators = 2;

        p.oscillators[0] = {Waveform::Square, 1.0f, 0.7f, 0.0f, 3, 0.02f};
        p.oscillators[1] = {Waveform::Saw, 0.5f, 0.4f, 0.0f, 2, 0.015f};

        p.ampEnvelope = {0.01f, 0.1f, 0.85f, 0.2f};
        p.filterEnvelope = {0.01f, 0.15f, 0.6f, 0.15f};

        p.filter = {FilterType::LowPass, 2500.0f, 4.0f, 0.7f};

        p.lfo = {Waveform::Sine, 0.0f, 0.0f, false, false, false};

        p.macros[0].name = "Drive";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 1000.0f, 6000.0f, MacroCurve::Exponential});

        p.macros[1].name = "Nasty";
        p.macros[1].targets.add({MacroParamType::FilterResonance, 3.0f, 7.0f, MacroCurve::Linear});

        p.macros[2].name = "Grit";
        p.macros[2].targets.add({MacroParamType::OscDetune, 0.01f, 0.04f, MacroCurve::Linear});
    }

    // Preset 64: "Random"
    {
        VoicePreset &p = presets[63];
        p.name = "FX: Random";
        p.numOscillators = 3;

        p.oscillators[0] = {Waveform::Saw, 1.0f, 0.4f, 0.0f, 3, 0.02f};
        p.oscillators[1] = {Waveform::Square, 1.5f, 0.35f, 0.0f, 2, 0.015f};
        p.oscillators[2] = {Waveform::Triangle, 0.75f, 0.25f, 0.0f, 2, 0.01f};

        p.ampEnvelope = {0.1f, 0.3f, 0.7f, 0.5f};
        p.filterEnvelope = {0.08f, 0.4f, 0.5f, 0.4f};

        p.filter = {FilterType::LowPass, 2500.0f, 2.0f, 0.5f};

        p.lfo = {Waveform::Triangle, 0.5f, 0.3f, true, true, false};

        p.macros[0].name = "Chaos";
        p.macros[0].targets.add({MacroParamType::FilterCutoff, 500.0f, 8000.0f, MacroCurve::Exponential});
        p.macros[0].targets.add({MacroParamType::LFORate, 0.1f, 3.0f, MacroCurve::Exponential});

        p.macros[1].name = "Mutate";
        p.macros[1].targets.add({MacroParamType::OscDetune, 0.01f, 0.06f, MacroCurve::Linear});
        p.macros[1].targets.add({MacroParamType::FilterResonance, 1.0f, 5.0f, MacroCurve::Linear});

        p.macros[2].name = "Morph";
        p.macros[2].targets.add({MacroParamType::LFODepth, 0.1f, 0.7f, MacroCurve::Linear});
        p.macros[2].targets.add({MacroParamType::EnvRelease, 0.2f, 1.5f, MacroCurve::Exponential});
    }

    initialized = true;
}

const VoicePreset &VoicePresets::getPreset(int index) {
    initializePresets();
    index = juce::jlimit(0, 63, index);
    return presets[index];
}

juce::String VoicePresets::getPresetName(int index) {
    return getPreset(index).name;
}

} // namespace internal_plugins
