#pragma once

#include <juce_core/juce_core.h>

namespace internal_plugins {

/**
 * Waveform types for oscillators and LFOs
 */
enum class Waveform { Sine = 0, Saw, Square, Triangle, Noise };

/**
 * Filter types
 */
enum class FilterType { LowPass = 0, HighPass, BandPass };

/**
 * Macro curve types for parameter modulation
 */
enum class MacroCurve { Linear = 0, Exponential, Logarithmic };

/**
 * Parameter types that macros can target
 */
enum class MacroParamType {
    FilterCutoff = 0,
    FilterResonance,
    OscDetune,
    OscLevel,
    EnvAttack,
    EnvRelease,
    LFORate,
    LFODepth
};

/**
 * Oscillator configuration
 */
struct OscillatorConfig {
    Waveform waveform = Waveform::Saw;
    float freqMultiplier = 1.0f;
    float level = 0.7f;
    float phase = 0.0f;
    int numVoices = 1;
    float detune = 0.0f;
};

/**
 * ADSR envelope configuration
 */
struct ADSRConfig {
    float attack = 0.1f;  // seconds
    float decay = 0.2f;   // seconds
    float sustain = 0.7f; // 0-1
    float release = 0.3f; // seconds
};

/**
 * Filter configuration
 */
struct FilterConfig {
    FilterType type = FilterType::LowPass;
    float cutoff = 2000.0f;     // Hz
    float resonance = 1.0f;     // Q factor
    float envAmount = 0.0f;     // -1 to +1
};

/**
 * LFO configuration
 */
struct LFOConfig {
    Waveform waveform = Waveform::Sine;
    float rate = 2.0f;          // Hz
    float depth = 0.0f;         // 0-1
    bool targetFilterCutoff = false;
    bool targetOscPitch = false;
    bool targetOscLevel = false;
};

/**
 * Macro target definition
 */
struct MacroTarget {
    MacroParamType paramType;
    float minValue;
    float maxValue;
    MacroCurve curve = MacroCurve::Linear;
};

/**
 * Macro definition with name and targets
 */
struct MacroDefinition {
    juce::String name;
    juce::Array<MacroTarget> targets;
};

/**
 * Complete voice preset definition
 */
struct VoicePreset {
    juce::String name;

    // Up to 4 oscillators
    OscillatorConfig oscillators[4];
    int numOscillators = 1;

    // Envelopes
    ADSRConfig ampEnvelope;
    ADSRConfig filterEnvelope;

    // Filter
    FilterConfig filter;

    // LFO
    LFOConfig lfo;

    // 7 macros per preset (1-3 primary, 4-7 secondary accessed via control button)
    MacroDefinition macros[7];
};

/**
 * VoicePresets - Class providing 128 GM-aligned voice presets loaded from XML
 */
class VoicePresets {
  public:
    /**
     * Get a preset by index (0-127)
     */
    static const VoicePreset &getPreset(int index);

    /**
     * Get the total number of presets
     */
    static int getNumPresets() { return 128; }

    /**
     * Get preset name by index
     */
    static juce::String getPresetName(int index);

    /**
     * Get category name for a preset index
     */
    static juce::String getCategoryName(int index);

  private:
    static VoicePreset presets[128];
    static juce::String categories[128];
    static bool initialized;
    static void initializePresets();
    static void loadPresetsFromXML();
    static VoicePreset parsePreset(const juce::XmlElement* presetElement);
};

} // namespace internal_plugins
