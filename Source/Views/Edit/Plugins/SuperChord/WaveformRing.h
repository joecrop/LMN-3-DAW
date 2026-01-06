#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

/**
 * WaveformRing - Circular waveform visualization around the screen edge
 *
 * Shows a radial waveform that:
 * - Pulses with note triggers
 * - Morphs shape based on pitch wheel position
 * - Uses encoder colors for sections
 */
class WaveformRing : public juce::Component, private juce::Timer {
public:
    WaveformRing();
    ~WaveformRing() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    // Set encoder colors
    void setEncoderColors(juce::Colour c1, juce::Colour c2, juce::Colour c3,
                          juce::Colour c4);

    // Pitch wheel affects the waveform shape (0-16383)
    void setPitchWheelPosition(int position);

    // Trigger a pulse animation
    void triggerPulse(float intensity);

private:
    void timerCallback() override;
    float getWaveformValue(float angle, float time);

    // Encoder colors for ring sections
    juce::Colour encoderColors[4] = {
        juce::Colour(0xFF458588),  // Blue
        juce::Colour(0xFF689d6a),  // Green
        juce::Colour(0xFFf9f5d7),  // White
        juce::Colour(0xFFcc241d)   // Red
    };

    // Animation state
    float animTime = 0.0f;
    float pulseIntensity = 0.0f;
    float pulseDecay = 0.0f;

    // Pitch wheel morphs the waveform (0.0 to 1.0)
    float pitchWheelNormalized = 0.5f;

    // Waveform samples for smooth rendering
    std::vector<float> waveformSamples;
    static constexpr int numSamples = 128;

    // Ring geometry
    float ringRadius = 0.0f;
    float ringThickness = 4.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformRing)
};
