#include "WaveformRing.h"

WaveformRing::WaveformRing() {
    waveformSamples.resize(numSamples, 0.0f);
    startTimerHz(60);
}

WaveformRing::~WaveformRing() {
    stopTimer();
}

void WaveformRing::paint(juce::Graphics &g) {
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();

    // Calculate ring radius (slightly inset from edge)
    float minDim = juce::jmin(bounds.getWidth(), bounds.getHeight());
    ringRadius = (minDim / 2.0f) - 20.0f;

    // Build the waveform path
    juce::Path waveformPath;
    bool pathStarted = false;

    for (int i = 0; i <= numSamples; ++i) {
        float angle = (static_cast<float>(i % numSamples) / numSamples) *
                      juce::MathConstants<float>::twoPi;

        // Get waveform displacement
        float displacement = getWaveformValue(angle, animTime);

        // Apply pulse intensity
        displacement *= (1.0f + pulseIntensity * 2.0f);

        // Calculate point on ring
        float r = ringRadius + displacement * 15.0f;
        float x = center.x + r * std::cos(angle - juce::MathConstants<float>::halfPi);
        float y = center.y + r * std::sin(angle - juce::MathConstants<float>::halfPi);

        if (!pathStarted) {
            waveformPath.startNewSubPath(x, y);
            pathStarted = true;
        } else {
            waveformPath.lineTo(x, y);
        }
    }

    waveformPath.closeSubPath();

    // Draw colored sections
    for (int section = 0; section < 4; ++section) {
        float startAngle = section * juce::MathConstants<float>::halfPi;
        float endAngle = startAngle + juce::MathConstants<float>::halfPi;

        // Create clip region for this section
        juce::Path sectionClip;
        sectionClip.addPieSegment(bounds.reduced(10), startAngle - juce::MathConstants<float>::halfPi,
                                  endAngle - juce::MathConstants<float>::halfPi, 0.0f);

        g.saveState();
        g.reduceClipRegion(sectionClip);

        // Draw the waveform section in this color
        juce::Colour sectionColor = encoderColors[section].withAlpha(0.6f + pulseIntensity * 0.4f);
        g.setColour(sectionColor);
        g.strokePath(waveformPath, juce::PathStrokeType(ringThickness + pulseIntensity * 2.0f));

        // Draw inner glow
        g.setColour(sectionColor.withAlpha(0.3f));
        g.strokePath(waveformPath, juce::PathStrokeType(ringThickness * 2.0f + pulseIntensity * 4.0f));

        g.restoreState();
    }

    // Draw subtle center circle
    float centerRadius = ringRadius * 0.1f;
    g.setColour(juce::Colours::white.withAlpha(0.1f + pulseIntensity * 0.1f));
    g.drawEllipse(center.x - centerRadius, center.y - centerRadius,
                  centerRadius * 2, centerRadius * 2, 1.0f);
}

void WaveformRing::resized() {
    float minDim = juce::jmin(getWidth(), getHeight());
    ringRadius = (minDim / 2.0f) - 20.0f;
}

void WaveformRing::setEncoderColors(juce::Colour c1, juce::Colour c2,
                                    juce::Colour c3, juce::Colour c4) {
    encoderColors[0] = c1;
    encoderColors[1] = c2;
    encoderColors[2] = c3;
    encoderColors[3] = c4;
}

void WaveformRing::setPitchWheelPosition(int position) {
    pitchWheelNormalized = static_cast<float>(position) / 16383.0f;
}

void WaveformRing::triggerPulse(float intensity) {
    pulseIntensity = juce::jlimit(0.0f, 1.0f, intensity);
    pulseDecay = pulseIntensity * 0.02f;
}

void WaveformRing::timerCallback() {
    // Update animation time
    animTime += 0.02f;
    if (animTime > 1000.0f)
        animTime = 0.0f;

    // Decay pulse
    if (pulseIntensity > 0) {
        pulseIntensity -= pulseDecay;
        if (pulseIntensity < 0)
            pulseIntensity = 0;
    }

    repaint();
}

float WaveformRing::getWaveformValue(float angle, float time) {
    // Base sine wave
    float value = std::sin(angle * 3.0f + time);

    // Add harmonics based on pitch wheel position
    // Low pitch wheel = simple wave, high = complex

    // Second harmonic
    float h2 = std::sin(angle * 5.0f - time * 1.2f) * 0.5f;
    value += h2 * pitchWheelNormalized;

    // Third harmonic (appears with more pitch wheel)
    if (pitchWheelNormalized > 0.3f) {
        float h3 = std::sin(angle * 7.0f + time * 0.8f) * 0.3f;
        value += h3 * (pitchWheelNormalized - 0.3f) / 0.7f;
    }

    // Fourth harmonic (high pitch wheel only)
    if (pitchWheelNormalized > 0.6f) {
        float h4 = std::sin(angle * 11.0f - time * 1.5f) * 0.2f;
        value += h4 * (pitchWheelNormalized - 0.6f) / 0.4f;
    }

    // Normalize
    float maxVal = 1.0f + 0.5f * pitchWheelNormalized + 
                   0.3f * juce::jmax(0.0f, pitchWheelNormalized - 0.3f) / 0.7f +
                   0.2f * juce::jmax(0.0f, pitchWheelNormalized - 0.6f) / 0.4f;

    return value / maxVal;
}
