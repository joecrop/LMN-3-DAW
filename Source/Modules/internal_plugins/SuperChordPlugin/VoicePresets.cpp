#include "VoicePresets.h"

namespace internal_plugins {

VoicePresets::VoicePresets() { initializePresets(); }

void VoicePresets::initializePresets() {
    presets.clear();

    // 0. Ethereal Pad - Soft, atmospheric, slow-moving
    {
        VoicePreset preset;
        preset.name = "Ethereal Pad";
        preset.sineLevel = 0.6f;
        preset.sawLevel = 0.2f;
        preset.triangleLevel = 0.2f;
        preset.squareLevel = 0.0f;
        preset.filterCutoff = 1500.0f;
        preset.filterResonance = 0.2f;
        preset.attackTime = 0.8f;
        preset.decayTime = 0.5f;
        preset.sustainLevel = 0.85f;
        preset.releaseTime = 2.0f;
        preset.detuneAmount = 0.015f;
        preset.stereoSpread = 0.7f;
        preset.brightness = 0.3f;
        presets.push_back(preset);
    }

    // 1. Crystal Keys - Bright, bell-like, percussive
    {
        VoicePreset preset;
        preset.name = "Crystal Keys";
        preset.sineLevel = 0.4f;
        preset.sawLevel = 0.1f;
        preset.triangleLevel = 0.5f;
        preset.squareLevel = 0.0f;
        preset.filterCutoff = 6000.0f;
        preset.filterResonance = 0.4f;
        preset.attackTime = 0.005f;
        preset.decayTime = 0.4f;
        preset.sustainLevel = 0.3f;
        preset.releaseTime = 0.8f;
        preset.detuneAmount = 0.008f;
        preset.stereoSpread = 0.5f;
        preset.brightness = 0.85f;
        presets.push_back(preset);
    }

    // 2. Warm Strings - Lush, analog, ensemble feel
    {
        VoicePreset preset;
        preset.name = "Warm Strings";
        preset.sineLevel = 0.2f;
        preset.sawLevel = 0.6f;
        preset.triangleLevel = 0.2f;
        preset.squareLevel = 0.0f;
        preset.filterCutoff = 2500.0f;
        preset.filterResonance = 0.25f;
        preset.attackTime = 0.3f;
        preset.decayTime = 0.3f;
        preset.sustainLevel = 0.75f;
        preset.releaseTime = 0.6f;
        preset.detuneAmount = 0.025f;
        preset.stereoSpread = 0.8f;
        preset.brightness = 0.4f;
        presets.push_back(preset);
    }

    // 3. Digital Bells - Metallic, FM-like, harmonic
    {
        VoicePreset preset;
        preset.name = "Digital Bells";
        preset.sineLevel = 0.5f;
        preset.sawLevel = 0.0f;
        preset.triangleLevel = 0.3f;
        preset.squareLevel = 0.2f;
        preset.filterCutoff = 8000.0f;
        preset.filterResonance = 0.5f;
        preset.attackTime = 0.001f;
        preset.decayTime = 1.5f;
        preset.sustainLevel = 0.0f;
        preset.releaseTime = 2.0f;
        preset.detuneAmount = 0.005f;
        preset.stereoSpread = 0.4f;
        preset.brightness = 0.9f;
        presets.push_back(preset);
    }

    // 4. Soft Rhodes - Electric piano, vintage warmth
    {
        VoicePreset preset;
        preset.name = "Soft Rhodes";
        preset.sineLevel = 0.7f;
        preset.sawLevel = 0.1f;
        preset.triangleLevel = 0.2f;
        preset.squareLevel = 0.0f;
        preset.filterCutoff = 3000.0f;
        preset.filterResonance = 0.15f;
        preset.attackTime = 0.01f;
        preset.decayTime = 0.8f;
        preset.sustainLevel = 0.4f;
        preset.releaseTime = 0.4f;
        preset.detuneAmount = 0.003f;
        preset.stereoSpread = 0.3f;
        preset.brightness = 0.5f;
        presets.push_back(preset);
    }

    // 5. Synth Brass - Bold, punchy, cutting
    {
        VoicePreset preset;
        preset.name = "Synth Brass";
        preset.sineLevel = 0.1f;
        preset.sawLevel = 0.8f;
        preset.triangleLevel = 0.0f;
        preset.squareLevel = 0.1f;
        preset.filterCutoff = 4000.0f;
        preset.filterResonance = 0.35f;
        preset.attackTime = 0.05f;
        preset.decayTime = 0.15f;
        preset.sustainLevel = 0.8f;
        preset.releaseTime = 0.2f;
        preset.detuneAmount = 0.02f;
        preset.stereoSpread = 0.6f;
        preset.brightness = 0.7f;
        presets.push_back(preset);
    }

    // 6. Glass Choir - Vocal, evolving, dreamy
    {
        VoicePreset preset;
        preset.name = "Glass Choir";
        preset.sineLevel = 0.5f;
        preset.sawLevel = 0.3f;
        preset.triangleLevel = 0.2f;
        preset.squareLevel = 0.0f;
        preset.filterCutoff = 2000.0f;
        preset.filterResonance = 0.45f;
        preset.attackTime = 0.5f;
        preset.decayTime = 0.4f;
        preset.sustainLevel = 0.7f;
        preset.releaseTime = 1.5f;
        preset.detuneAmount = 0.018f;
        preset.stereoSpread = 0.9f;
        preset.brightness = 0.45f;
        preset.pulseWidth = 0.3f;
        presets.push_back(preset);
    }

    // 7. Velvet Bass - Deep, round, supportive
    {
        VoicePreset preset;
        preset.name = "Velvet Bass";
        preset.sineLevel = 0.8f;
        preset.sawLevel = 0.2f;
        preset.triangleLevel = 0.0f;
        preset.squareLevel = 0.0f;
        preset.filterCutoff = 800.0f;
        preset.filterResonance = 0.2f;
        preset.attackTime = 0.02f;
        preset.decayTime = 0.3f;
        preset.sustainLevel = 0.6f;
        preset.releaseTime = 0.3f;
        preset.detuneAmount = 0.01f;
        preset.stereoSpread = 0.2f;
        preset.brightness = 0.25f;
        presets.push_back(preset);
    }
}

const VoicePreset &VoicePresets::getPreset(int index) const {
    int safeIndex = juce::jlimit(0, static_cast<int>(presets.size()) - 1, index);
    return presets[static_cast<size_t>(safeIndex)];
}

const VoicePreset *VoicePresets::getPresetByName(const juce::String &name) const {
    for (const auto &preset : presets) {
        if (preset.name == name) {
            return &preset;
        }
    }
    return nullptr;
}

juce::StringArray VoicePresets::getPresetNames() const {
    juce::StringArray names;
    for (const auto &preset : presets) {
        names.add(preset.name);
    }
    return names;
}

VoicePreset VoicePresets::interpolate(int fromIndex, int toIndex,
                                      float amount) const {
    const auto &from = getPreset(fromIndex);
    const auto &to = getPreset(toIndex);

    VoicePreset result;
    result.name = amount < 0.5f ? from.name : to.name;

    auto lerp = [](float a, float b, float t) { return a + t * (b - a); };

    result.sineLevel = lerp(from.sineLevel, to.sineLevel, amount);
    result.sawLevel = lerp(from.sawLevel, to.sawLevel, amount);
    result.triangleLevel = lerp(from.triangleLevel, to.triangleLevel, amount);
    result.squareLevel = lerp(from.squareLevel, to.squareLevel, amount);
    result.filterCutoff = lerp(from.filterCutoff, to.filterCutoff, amount);
    result.filterResonance =
        lerp(from.filterResonance, to.filterResonance, amount);
    result.attackTime = lerp(from.attackTime, to.attackTime, amount);
    result.decayTime = lerp(from.decayTime, to.decayTime, amount);
    result.sustainLevel = lerp(from.sustainLevel, to.sustainLevel, amount);
    result.releaseTime = lerp(from.releaseTime, to.releaseTime, amount);
    result.detuneAmount = lerp(from.detuneAmount, to.detuneAmount, amount);
    result.stereoSpread = lerp(from.stereoSpread, to.stereoSpread, amount);
    result.brightness = lerp(from.brightness, to.brightness, amount);
    result.pulseWidth = lerp(from.pulseWidth, to.pulseWidth, amount);

    return result;
}

} // namespace internal_plugins
