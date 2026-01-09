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
}

void SuperChordVisualizerPage::drawChordConstellation(juce::Graphics &g,
                                                      float centerX,
                                                      float centerY,
                                                      float radius) {
    int currentChord = plugin->getCurrentChordDegree();
    static const char *romanNumerals[] = {"I",   "ii",  "iii", "IV",
                                          "V",   "vi",  "vii°"};

    // Much larger dots - scale with window size
    float activeDotSize = juce::jmax(radius * 0.35f, 24.0f);
    float inactiveDotSize = juce::jmax(radius * 0.25f, 16.0f);
    float glowSize = activeDotSize * 1.5f;
    float labelFontSize = juce::jmax(getHeight() * 0.08f, 16.0f);

    // Draw 7 chord degree points
    for (int i = 0; i < 7; i++) {
        float angle =
            (i / 7.0f) * juce::MathConstants<float>::twoPi - 
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

        // Draw Roman numeral with larger font
        g.setColour(i == currentChord ? appLookAndFeel.whiteColour
                                      : juce::Colours::lightgrey);
        g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), labelFontSize,
                             juce::Font::bold));

        float labelOffset = activeDotSize * 0.8f + 10.0f;
        float labelX = centerX + std::cos(angle) * (radius + labelOffset);
        float labelY = centerY + std::sin(angle) * (radius + labelOffset);
        g.drawText(romanNumerals[i], 
                   static_cast<int>(labelX - 25),
                   static_cast<int>(labelY - 12), 50, 24,
                   juce::Justification::centred);
    }
}
