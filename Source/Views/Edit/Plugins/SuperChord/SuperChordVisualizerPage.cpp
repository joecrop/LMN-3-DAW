#include "SuperChordVisualizerPage.h"

SuperChordVisualizerPage::SuperChordVisualizerPage(
    internal_plugins::SuperChordPlugin *p,
    app_services::MidiCommandManager &mcm)
    : plugin(p), midiCommandManager(mcm) {
    // Preset name label - font will be set in resized()
    presetNameLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), 24, juce::Font::bold));
    presetNameLabel.setJustificationType(juce::Justification::centred);
    presetNameLabel.setColour(juce::Label::textColourId,
                              appLookAndFeel.blueColour);
    addAndMakeVisible(presetNameLabel);

    // Chord name label - font will be set in resized()
    chordNameLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), 36, juce::Font::bold));
    chordNameLabel.setJustificationType(juce::Justification::centred);
    chordNameLabel.setColour(juce::Label::textColourId,
                             appLookAndFeel.whiteColour);
    addAndMakeVisible(chordNameLabel);

    // Key signature label - font will be set in resized()
    keySignatureLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), 24, juce::Font::bold));
    keySignatureLabel.setJustificationType(juce::Justification::left);
    keySignatureLabel.setColour(juce::Label::textColourId,
                                appLookAndFeel.yellowColour);
    addAndMakeVisible(keySignatureLabel);

    // Progression label - shows current progression type
    progressionLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), 20, juce::Font::bold));
    progressionLabel.setJustificationType(juce::Justification::right);
    progressionLabel.setColour(juce::Label::textColourId,
                               appLookAndFeel.colour1);
    addAndMakeVisible(progressionLabel);

    midiCommandManager.addListener(this);

    // Start timer for animations (30 FPS)
    startTimerHz(30);
}

SuperChordVisualizerPage::~SuperChordVisualizerPage() {
    stopTimer();
    midiCommandManager.removeListener(this);
}

void SuperChordVisualizerPage::timerCallback() {
    // Update labels from plugin state
    presetNameLabel.setText(plugin->getCurrentPresetName(),
                            juce::dontSendNotification);

    juce::String chordName = plugin->getCurrentChordName();
    if (chordName.isEmpty()) {
        chordNameLabel.setText("Ready", juce::dontSendNotification);
    } else {
        chordNameLabel.setText(chordName, juce::dontSendNotification);
    }

    keySignatureLabel.setText("Key: " + plugin->getKeySignature(),
                              juce::dontSendNotification);

    // Update progression label
    auto progression = static_cast<internal_plugins::ProgressionType>(plugin->getProgressionValue());
    progressionLabel.setText(internal_plugins::ChordEngine::getProgressionName(progression),
                             juce::dontSendNotification);

    repaint();
}

void SuperChordVisualizerPage::paint(juce::Graphics &g) {
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    float centerX = getWidth() * 0.5f;
    float centerY = getHeight() * 0.55f;

    // Draw chord constellation (much larger dots)
    float radius = juce::jmin(getWidth(), getHeight()) * 0.25f;
    drawChordConstellation(g, centerX, centerY, radius);
}

void SuperChordVisualizerPage::resized() {
    // Use height-based font sizes to match 4OSC minimum
    float titleFontSize = juce::jmax(getHeight() * 0.1f, 20.0f);
    float chordFontSize = juce::jmax(getHeight() * 0.1f, 20.0f);  // Smaller, matches title
    float keyFontSize = juce::jmax(getHeight() * 0.1f, 20.0f);
    float progFontSize = juce::jmax(getHeight() * 0.08f, 16.0f);

    // Preset name label at top center
    presetNameLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), titleFontSize, juce::Font::bold));
    presetNameLabel.setBounds(0, getHeight() * 0.05f, getWidth(), getHeight() * 0.12f);

    // Chord name label in center of screen
    chordNameLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), chordFontSize, juce::Font::bold));
    chordNameLabel.setBounds(0, getHeight() * 0.45f, getWidth(), getHeight() * 0.12f);

    // Key signature label at bottom left
    keySignatureLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), keyFontSize, juce::Font::bold));
    keySignatureLabel.setBounds(10, getHeight() - getHeight() * 0.15f, getWidth() / 2, getHeight() * 0.12f);

    // Progression label at bottom right
    progressionLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), progFontSize, juce::Font::bold));
    progressionLabel.setBounds(getWidth() / 2, getHeight() - getHeight() * 0.15f, getWidth() / 2 - 10, getHeight() * 0.12f);
}

void SuperChordVisualizerPage::drawChordConstellation(juce::Graphics &g,
                                                      float centerX,
                                                      float centerY,
                                                      float radius) {
    int currentChord = plugin->getCurrentChordDegree();
    
    // Get progression-specific information
    auto progression = static_cast<internal_plugins::ProgressionType>(plugin->getProgressionValue());
    int numDegrees = internal_plugins::ChordEngine::getNumDegrees(progression);

    // Much larger dots - scale with window size
    float activeDotSize = juce::jmax(radius * 0.35f, 24.0f);
    float inactiveDotSize = juce::jmax(radius * 0.25f, 16.0f);
    float glowSize = activeDotSize * 1.5f;
    float labelFontSize = juce::jmax(getHeight() * 0.06f, 14.0f);

    // Draw chord degree points (variable count based on progression)
    for (int i = 0; i < numDegrees; i++) {
        // Space dots evenly around the circle
        float angle =
            (static_cast<float>(i) / static_cast<float>(numDegrees)) * juce::MathConstants<float>::twoPi - 
            juce::MathConstants<float>::halfPi;
        float x = centerX + std::cos(angle) * radius;
        float y = centerY + std::sin(angle) * radius;

        if (i == currentChord) {
            // Active chord - yellow glow (much larger)
            g.setColour(appLookAndFeel.yellowColour.withAlpha(0.3f));
            g.fillEllipse(x - glowSize / 2, y - glowSize / 2, glowSize, glowSize);
            g.setColour(appLookAndFeel.yellowColour);
            g.fillEllipse(x - activeDotSize / 2, y - activeDotSize / 2, activeDotSize, activeDotSize);
        } else {
            // Inactive chord - larger dim point
            g.setColour(juce::Colours::grey);
            g.fillEllipse(x - inactiveDotSize / 2, y - inactiveDotSize / 2, inactiveDotSize, inactiveDotSize);
        }

        // Get progression-specific label for this degree
        juce::String degreeLabel = internal_plugins::ChordEngine::getDegreeLabel(progression, i);

        // Draw degree label with larger font
        g.setColour(i == currentChord ? appLookAndFeel.whiteColour
                                      : juce::Colours::lightgrey);
        g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), labelFontSize,
                             juce::Font::bold));

        float labelOffset = activeDotSize * 0.8f + 10.0f;
        float labelX = centerX + std::cos(angle) * (radius + labelOffset);
        float labelY = centerY + std::sin(angle) * (radius + labelOffset);
        g.drawText(degreeLabel, 
                   static_cast<int>(labelX - 30),
                   static_cast<int>(labelY - 12), 60, 24,
                   juce::Justification::centred);
    }
}

void SuperChordVisualizerPage::encoder1Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getProgressionValue();
        int maxValue = static_cast<int>(internal_plugins::ProgressionType::NumProgressions) - 1;
        if (currentValue < maxValue) {
            plugin->progressionParam->setParameter(
                static_cast<float>(currentValue + 1), juce::sendNotification);
        }
    }
}

void SuperChordVisualizerPage::encoder1Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getProgressionValue();
        if (currentValue > 0) {
            plugin->progressionParam->setParameter(
                static_cast<float>(currentValue - 1), juce::sendNotification);
        }
    }
}
