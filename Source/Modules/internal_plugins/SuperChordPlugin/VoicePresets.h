#pragma once

#include <juce_core/juce_core.h>
#include <vector>

namespace internal_plugins {

/**
 * VoicePreset - Defines the sonic character of a voice preset.
 *
 * Each preset provides base values for oscillator mix, filter settings,
 * and envelope shapes. The macro parameters (Warmth, Space, Attack, Bloom)
 * then modify these base values to create variations.
 */
struct VoicePreset {
    juce::String name;

    // Oscillator levels (0.0 - 1.0)
    float sineLevel = 0.5f;
    float sawLevel = 0.3f;
    float triangleLevel = 0.2f;
    float squareLevel = 0.0f;

    // Filter settings
    float filterCutoff = 2000.0f;    // Hz
    float filterResonance = 0.3f;    // 0.0 - 1.0

    // Envelope base settings
    float attackTime = 0.1f;         // seconds
    float decayTime = 0.2f;          // seconds
    float sustainLevel = 0.7f;       // 0.0 - 1.0
    float releaseTime = 0.5f;        // seconds

    // Character modifiers
    float detuneAmount = 0.01f;      // 0.0 - 0.05 (percentage)
    float stereoSpread = 0.3f;       // 0.0 - 1.0
    float brightness = 0.5f;         // 0.0 - 1.0 (affects filter)

    // Pulse width for square wave
    float pulseWidth = 0.5f;         // 0.0 - 1.0
};

/**
 * VoicePresets - Collection of curated voice presets.
 *
 * Presets:
 * 0. Ethereal Pad - Soft, atmospheric, slow-moving
 * 1. Crystal Keys - Bright, bell-like, percussive
 * 2. Warm Strings - Lush, analog, ensemble feel
 * 3. Digital Bells - Metallic, FM-like, harmonic
 * 4. Soft Rhodes - Electric piano, vintage warmth
 * 5. Synth Brass - Bold, punchy, cutting
 * 6. Glass Choir - Vocal, evolving, dreamy
 * 7. Velvet Bass - Deep, round, supportive
 */
class VoicePresets {
  public:
    VoicePresets();

    // Get preset by index
    const VoicePreset &getPreset(int index) const;

    // Get preset by name
    const VoicePreset *getPresetByName(const juce::String &name) const;

    // Get number of presets
    int getNumPresets() const { return static_cast<int>(presets.size()); }

    // Get all preset names
    juce::StringArray getPresetNames() const;

    // Interpolate between two presets (for crossfading)
    VoicePreset interpolate(int fromIndex, int toIndex, float amount) const;

  private:
    void initializePresets();

    std::vector<VoicePreset> presets;
};

} // namespace internal_plugins
