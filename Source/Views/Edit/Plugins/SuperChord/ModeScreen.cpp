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

    if (currentLevel == 0) {
        drawCategoryOrbs(g);
    } else {
        drawItemOrbs(g);
    }
}

void ModeScreen::resized() {
    // Nothing specific needed
}

void ModeScreen::navigateNext() {
    selectionPulse = 1.0f;

    if (currentLevel == 0) {
        selectedCategory = (selectedCategory + 1) % numCategories;
    } else {
        int maxItems = 0;
        switch (selectedCategory) {
            case 0: maxItems = numScales; break;
            case 1: maxItems = numPlayModes; break;
            case 2: maxItems = numVoicePresets; break;
        }
        selectedItem = (selectedItem + 1) % maxItems;
    }
    repaint();
}

void ModeScreen::navigatePrevious() {
    selectionPulse = 1.0f;

    if (currentLevel == 0) {
        selectedCategory = (selectedCategory + numCategories - 1) % numCategories;
    } else {
        int maxItems = 0;
        switch (selectedCategory) {
            case 0: maxItems = numScales; break;
            case 1: maxItems = numPlayModes; break;
            case 2: maxItems = numVoicePresets; break;
        }
        selectedItem = (selectedItem + maxItems - 1) % maxItems;
    }
    repaint();
}

void ModeScreen::selectCurrent() {
    selectionPulse = 1.0f;

    if (currentLevel == 0) {
        // Enter category
        currentLevel = 1;
        selectedItem = 0;
    } else {
        // Confirm selection and notify listeners
        for (auto *l : listeners) {
            switch (selectedCategory) {
                case 0: l->scaleTypeChanged(selectedItem); break;
                case 1: l->playModeChanged(selectedItem); break;
                case 2: l->voicePresetChanged(selectedItem); break;
            }
        }
        // Return to top level
        currentLevel = 0;
    }
    repaint();
}

void ModeScreen::goBack() {
    selectionPulse = 1.0f;

    if (currentLevel > 0) {
        currentLevel = 0;
    }
    repaint();
}

void ModeScreen::reset() {
    currentLevel = 0;
    selectedCategory = 0;
    selectedItem = 0;
    repaint();
}

void ModeScreen::timerCallback() {
    animTime += 0.016f;

    // Decay selection pulse
    if (selectionPulse > 0) {
        selectionPulse -= 0.05f;
        if (selectionPulse < 0)
            selectionPulse = 0;
    }

    repaint();
}

void ModeScreen::drawCategoryOrbs(juce::Graphics &g) {
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();

    float orbRadius = 30.0f;
    float spacing = 100.0f;

    // Draw three category orbs horizontally
    for (int i = 0; i < numCategories; ++i) {
        float x = center.x + (i - 1) * spacing;
        float y = center.y;

        bool isSelected = (i == selectedCategory);
        juce::Colour orbColor = getCategoryColor(i);

        // Pulsing effect for selected orb
        float pulse = isSelected ? (1.0f + std::sin(animTime * 3.0f) * 0.2f + selectionPulse * 0.3f) : 0.8f;
        float size = orbRadius * pulse;

        // Draw glow for selected
        if (isSelected) {
            float glowSize = size * 2.5f;
            g.setColour(orbColor.withAlpha(0.3f));
            g.fillEllipse(x - glowSize / 2, y - glowSize / 2, glowSize, glowSize);
        }

        // Draw orb
        g.setColour(orbColor.withAlpha(isSelected ? 1.0f : 0.5f));
        g.fillEllipse(x - size / 2, y - size / 2, size, size);

        // Draw bright center
        float centerSize = size * 0.4f;
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.fillEllipse(x - centerSize / 2, y - centerSize / 2, centerSize, centerSize);

        // Draw small indicator orbs below (representing items in category)
        int numItems = 0;
        switch (i) {
            case 0: numItems = numScales; break;
            case 1: numItems = numPlayModes; break;
            case 2: numItems = numVoicePresets; break;
        }

        float indicatorSpacing = 10.0f;
        float indicatorStartX = x - (numItems - 1) * indicatorSpacing / 2;
        for (int j = 0; j < numItems; ++j) {
            float ix = indicatorStartX + j * indicatorSpacing;
            float iy = y + orbRadius + 20.0f;
            g.setColour(orbColor.withAlpha(0.3f));
            g.fillEllipse(ix - 3, iy - 3, 6, 6);
        }
    }
}

void ModeScreen::drawItemOrbs(juce::Graphics &g) {
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();

    int numItems = 0;
    switch (selectedCategory) {
        case 0: numItems = numScales; break;
        case 1: numItems = numPlayModes; break;
        case 2: numItems = numVoicePresets; break;
    }

    juce::Colour baseColor = getCategoryColor(selectedCategory);
    float orbRadius = 25.0f;

    // Arrange items in a circle
    float circleRadius = 80.0f;
    for (int i = 0; i < numItems; ++i) {
        float angle = (static_cast<float>(i) / numItems) * juce::MathConstants<float>::twoPi -
                      juce::MathConstants<float>::halfPi;
        float x = center.x + std::cos(angle) * circleRadius;
        float y = center.y + std::sin(angle) * circleRadius;

        bool isSelected = (i == selectedItem);

        // Pulsing for selected
        float pulse = isSelected ? (1.0f + std::sin(animTime * 3.0f) * 0.2f + selectionPulse * 0.3f) : 0.7f;
        float size = orbRadius * pulse;

        // Slight color variation per item
        juce::Colour orbColor = baseColor.withRotatedHue(i * 0.05f);

        // Draw glow for selected
        if (isSelected) {
            float glowSize = size * 2.5f;
            g.setColour(orbColor.withAlpha(0.3f));
            g.fillEllipse(x - glowSize / 2, y - glowSize / 2, glowSize, glowSize);
        }

        // Draw orb
        g.setColour(orbColor.withAlpha(isSelected ? 1.0f : 0.4f));
        g.fillEllipse(x - size / 2, y - size / 2, size, size);

        // Draw center
        float centerSize = size * 0.4f;
        g.setColour(juce::Colours::white.withAlpha(isSelected ? 0.6f : 0.2f));
        g.fillEllipse(x - centerSize / 2, y - centerSize / 2, centerSize, centerSize);
    }

    // Draw category indicator in center (smaller, dimmer)
    float catSize = 15.0f;
    g.setColour(baseColor.withAlpha(0.4f));
    g.fillEllipse(center.x - catSize / 2, center.y - catSize / 2, catSize, catSize);
}

juce::Colour ModeScreen::getCategoryColor(int category) {
    switch (category) {
        case 0: return scaleColor;       // Scale = Blue
        case 1: return playModeColor;    // Play Mode = Green
        case 2: return voicePresetColor; // Voice Preset = Red
        default: return juce::Colours::white;
    }
}
