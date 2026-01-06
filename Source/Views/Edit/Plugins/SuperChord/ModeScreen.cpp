#include "ModeScreen.h"

ModeScreen::ModeScreen() {
    startTimerHz(60);
}

ModeScreen::~ModeScreen() {
    stopTimer();
}

void ModeScreen::paint(juce::Graphics &g) {
    // Semi-transparent dark overlay
    g.fillAll(juce::Colour(0xE0282828));
    
    drawSettingsPanel(g);
}

void ModeScreen::resized() {
    // Nothing specific needed
}

void ModeScreen::nextScale() {
    currentScale = (currentScale + 1) % numScales;
    scalePulse = 1.0f;
    for (auto *l : listeners) {
        l->scaleTypeChanged(currentScale);
    }
    repaint();
}

void ModeScreen::previousScale() {
    currentScale = (currentScale + numScales - 1) % numScales;
    scalePulse = 1.0f;
    for (auto *l : listeners) {
        l->scaleTypeChanged(currentScale);
    }
    repaint();
}

void ModeScreen::nextPlayMode() {
    currentPlayMode = (currentPlayMode + 1) % numPlayModes;
    playModePulse = 1.0f;
    for (auto *l : listeners) {
        l->playModeChanged(currentPlayMode);
    }
    repaint();
}

void ModeScreen::previousPlayMode() {
    currentPlayMode = (currentPlayMode + numPlayModes - 1) % numPlayModes;
    playModePulse = 1.0f;
    for (auto *l : listeners) {
        l->playModeChanged(currentPlayMode);
    }
    repaint();
}

void ModeScreen::nextVoicePreset() {
    currentVoicePreset = (currentVoicePreset + 1) % numVoicePresets;
    voicePulse = 1.0f;
    for (auto *l : listeners) {
        l->voicePresetChanged(currentVoicePreset);
    }
    repaint();
}

void ModeScreen::previousVoicePreset() {
    currentVoicePreset = (currentVoicePreset + numVoicePresets - 1) % numVoicePresets;
    voicePulse = 1.0f;
    for (auto *l : listeners) {
        l->voicePresetChanged(currentVoicePreset);
    }
    repaint();
}

void ModeScreen::timerCallback() {
    animTime += 0.016f;

    // Decay pulses
    if (scalePulse > 0) {
        scalePulse -= 0.05f;
        if (scalePulse < 0) scalePulse = 0;
    }
    if (playModePulse > 0) {
        playModePulse -= 0.05f;
        if (playModePulse < 0) playModePulse = 0;
    }
    if (voicePulse > 0) {
        voicePulse -= 0.05f;
        if (voicePulse < 0) voicePulse = 0;
    }

    repaint();
}

void ModeScreen::drawSettingsPanel(juce::Graphics &g) {
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(28.0f).boldened());
    g.drawText("SETTINGS", bounds.withHeight(50.0f).withY(15.0f), 
               juce::Justification::centredTop);

    // Layout: three rows, each showing label + value with colored orb
    float rowHeight = 70.0f;
    float startY = center.y - rowHeight * 1.5f;
    float orbRadius = 18.0f;
    float orbX = bounds.getWidth() * 0.15f;
    float labelX = bounds.getWidth() * 0.22f;
    float valueX = bounds.getWidth() * 0.5f;

    // Helper lambda to draw a setting row
    auto drawSettingRow = [&](int rowIndex, const juce::String& label, 
                               const juce::String& value, juce::Colour color, float pulse) {
        float y = startY + rowIndex * rowHeight + rowHeight / 2;
        
        // Calculate pulse effect
        float pulseScale = 1.0f + pulse * 0.3f;
        float glowAlpha = 0.3f + pulse * 0.4f;
        
        // Draw orb with glow
        float orbSize = orbRadius * pulseScale;
        
        // Glow
        float glowSize = orbSize * 2.5f;
        g.setColour(color.withAlpha(glowAlpha));
        g.fillEllipse(orbX - glowSize / 2, y - glowSize / 2, glowSize, glowSize);
        
        // Orb
        g.setColour(color);
        g.fillEllipse(orbX - orbSize / 2, y - orbSize / 2, orbSize, orbSize);
        
        // Bright center
        float centerSize = orbSize * 0.4f;
        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.fillEllipse(orbX - centerSize / 2, y - centerSize / 2, centerSize, centerSize);
        
        // Label (setting name)
        g.setColour(color.brighter(0.3f));
        g.setFont(juce::Font(22.0f).boldened());
        g.drawText(label, 
                   juce::Rectangle<float>(labelX, y - 15, 120, 30),
                   juce::Justification::centredLeft);
        
        // Value with highlight on pulse
        g.setColour(juce::Colours::white.withAlpha(0.85f + pulse * 0.15f));
        g.setFont(juce::Font(24.0f).boldened());
        g.drawText(value,
                   juce::Rectangle<float>(valueX, y - 15, bounds.getWidth() - valueX - 20, 30),
                   juce::Justification::centredLeft);
        
        // Separator line
        if (rowIndex < 2) {
            g.setColour(juce::Colours::grey.withAlpha(0.3f));
            g.drawLine(orbX, y + rowHeight / 2 - 5, 
                       bounds.getWidth() - 20, y + rowHeight / 2 - 5, 1.0f);
        }
    };

    // Draw the three settings
    drawSettingRow(0, "SCALE", getItemName(0, currentScale), scaleColor, scalePulse);
    drawSettingRow(1, "MODE", getItemName(1, currentPlayMode), playModeColor, playModePulse);
    drawSettingRow(2, "VOICE", getItemName(2, currentVoicePreset), voicePresetColor, voicePulse);

    // Encoder hints at bottom
    g.setFont(juce::Font(16.0f));
    float hintY = bounds.getHeight() - 50.0f;
    float hintWidth = bounds.getWidth() / 3.0f;
    
    g.setColour(scaleColor);
    g.drawText("< Enc1 >", 
               juce::Rectangle<float>(0, hintY, hintWidth, 25),
               juce::Justification::centred);
    g.setFont(juce::Font(14.0f));
    g.drawText("Scale", 
               juce::Rectangle<float>(0, hintY + 22, hintWidth, 20),
               juce::Justification::centred);
    
    g.setFont(juce::Font(16.0f));
    g.setColour(playModeColor);
    g.drawText("< Enc2 >", 
               juce::Rectangle<float>(hintWidth, hintY, hintWidth, 25),
               juce::Justification::centred);
    g.setFont(juce::Font(14.0f));
    g.drawText("Mode", 
               juce::Rectangle<float>(hintWidth, hintY + 22, hintWidth, 20),
               juce::Justification::centred);
    
    g.setFont(juce::Font(16.0f));
    g.setColour(voicePresetColor);
    g.drawText("< Enc4 >", 
               juce::Rectangle<float>(hintWidth * 2, hintY, hintWidth, 25),
               juce::Justification::centred);
    g.setFont(juce::Font(14.0f));
    g.drawText("Voice", 
               juce::Rectangle<float>(hintWidth * 2, hintY + 22, hintWidth, 20),
               juce::Justification::centred);
    
    // Close hint
    g.setColour(juce::Colours::grey);
    g.setFont(juce::Font(14.0f));
    g.drawText("Press Enc1 to close",
               bounds.withHeight(25.0f).withY(bounds.getHeight() - 20.0f),
               juce::Justification::centredBottom);
}

juce::Colour ModeScreen::getCategoryColor(int category) {
    switch (category) {
        case 0: return scaleColor;       // Scale = Blue
        case 1: return playModeColor;    // Play Mode = Green
        case 2: return voicePresetColor; // Voice Preset = Red
        default: return juce::Colours::white;
    }
}

juce::String ModeScreen::getCategoryName(int category) {
    switch (category) {
        case 0: return "SCALE";
        case 1: return "MODE";
        case 2: return "VOICE";
        default: return "Unknown";
    }
}

juce::String ModeScreen::getItemName(int category, int item) {
    if (category == 0) {
        // Scale types
        switch (item) {
            case 0: return "Major";
            case 1: return "Minor";
            case 2: return "Dorian";
            case 3: return "Mixolydian";
            case 4: return "Lydian";
            case 5: return "Phrygian";
            case 6: return "Locrian";
            default: return "Unknown";
        }
    } else if (category == 1) {
        // Play modes
        switch (item) {
            case 0: return "Chord";
            case 1: return "Strum";
            case 2: return "Arpeggio";
            default: return "Unknown";
        }
    } else if (category == 2) {
        // Voice presets
        switch (item) {
            case 0: return "Ethereal Pad";
            case 1: return "Crystal Keys";
            case 2: return "Warm Strings";
            case 3: return "Digital Bells";
            case 4: return "Soft Rhodes";
            case 5: return "Synth Brass";
            case 6: return "Glass Choir";
            case 7: return "Velvet Bass";
            default: return "Unknown";
        }
    }
    return "Unknown";
}
