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

    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(18.0f).boldened());
    g.drawText("Settings", bounds.withHeight(40.0f).withY(20.0f), 
               juce::Justification::centredTop);

    // Draw three category orbs horizontally
    for (int i = 0; i < numCategories; ++i) {
        float x = center.x + (i - 1) * spacing;
        float y = center.y - 20.0f;

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

        // Draw category name below orb
        g.setColour(isSelected ? juce::Colours::white : juce::Colours::grey);
        g.setFont(juce::Font(isSelected ? 14.0f : 12.0f));
        g.drawText(getCategoryName(i), 
                   juce::Rectangle<float>(x - 50, y + orbRadius + 10, 100, 20),
                   juce::Justification::centred);

        // Draw small indicator orbs below text (representing items in category)
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
            float iy = y + orbRadius + 35.0f;
            g.setColour(orbColor.withAlpha(0.3f));
            g.fillEllipse(ix - 3, iy - 3, 6, 6);
        }
    }

    // Draw hint at bottom
    g.setColour(juce::Colours::grey);
    g.setFont(juce::Font(11.0f));
    g.drawText("Turn encoder to select, press to enter",
               bounds.withHeight(20.0f).withY(bounds.getHeight() - 30.0f),
               juce::Justification::centredBottom);
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
    float orbRadius = 20.0f;

    // Draw category title at top
    g.setColour(baseColor);
    g.setFont(juce::Font(18.0f).boldened());
    g.drawText(getCategoryName(selectedCategory), 
               bounds.withHeight(40.0f).withY(20.0f),
               juce::Justification::centredTop);

    // Calculate layout - use vertical list for better text display
    float itemHeight = 35.0f;
    float listHeight = numItems * itemHeight;
    float startY = center.y - listHeight / 2;

    for (int i = 0; i < numItems; ++i) {
        float y = startY + i * itemHeight + itemHeight / 2;
        bool isSelected = (i == selectedItem);

        // Pulsing for selected
        float pulse = isSelected ? (1.0f + std::sin(animTime * 3.0f) * 0.1f + selectionPulse * 0.2f) : 1.0f;

        // Slight color variation per item
        juce::Colour orbColor = baseColor.withRotatedHue(i * 0.03f);

        // Draw selection background
        if (isSelected) {
            g.setColour(orbColor.withAlpha(0.2f));
            g.fillRoundedRectangle(center.x - 120, y - 14, 240, 28, 5.0f);
        }

        // Draw small orb indicator
        float orbSize = orbRadius * (isSelected ? pulse : 0.6f);
        float orbX = center.x - 100;
        
        if (isSelected) {
            // Draw glow for selected
            float glowSize = orbSize * 2.0f;
            g.setColour(orbColor.withAlpha(0.3f));
            g.fillEllipse(orbX - glowSize / 2, y - glowSize / 2, glowSize, glowSize);
        }

        g.setColour(orbColor.withAlpha(isSelected ? 1.0f : 0.4f));
        g.fillEllipse(orbX - orbSize / 2, y - orbSize / 2, orbSize, orbSize);

        // Draw item name
        g.setColour(isSelected ? juce::Colours::white : juce::Colours::grey);
        g.setFont(juce::Font(isSelected ? 15.0f : 13.0f));
        g.drawText(getItemName(selectedCategory, i),
                   juce::Rectangle<float>(center.x - 70, y - 10, 180, 20),
                   juce::Justification::centredLeft);
    }

    // Draw hint at bottom
    g.setColour(juce::Colours::grey);
    g.setFont(juce::Font(11.0f));
    g.drawText("Turn to select, press to confirm, Enc4 to go back",
               bounds.withHeight(20.0f).withY(bounds.getHeight() - 30.0f),
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
        case 0: return "Scale";
        case 1: return "Play Mode";
        case 2: return "Voice";
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
