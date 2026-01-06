#include "SuperChordView.h"

SuperChordView::SuperChordView(internal_plugins::SuperChordPlugin *p,
                               app_services::MidiCommandManager &mcm)
    : plugin(p), midiCommandManager(mcm) {
    setLookAndFeel(&appLookAndFeel);

    // Create visual components
    particleSystem = std::make_unique<ParticleSystem>();
    waveformRing = std::make_unique<WaveformRing>();
    visualizer = std::make_unique<Visualizer>();
    modeScreen = std::make_unique<ModeScreen>();

    addAndMakeVisible(particleSystem.get());
    addAndMakeVisible(waveformRing.get());
    addAndMakeVisible(visualizer.get());
    addChildComponent(modeScreen.get()); // Hidden initially

    // Set encoder colors
    particleSystem->setEncoderColors(encoder1Color, encoder2Color,
                                     encoder3Color, encoder4Color);
    waveformRing->setEncoderColors(encoder1Color, encoder2Color, encoder3Color,
                                   encoder4Color);
    visualizer->setEncoderColors(encoder1Color, encoder2Color, encoder3Color,
                                 encoder4Color);

    // Register as plugin listener
    plugin->addListener(this);

    // Register as mode screen listener to receive setting changes
    modeScreen->addListener(this);

    // Register for MIDI commands
    midiCommandManager.addListener(this);

    // Start animation timer (60 fps)
    startTimerHz(60);

    // Initialize from current plugin state
    updateVisualizationsFromParameters();
    currentChordName = plugin->getCurrentChordName();
}

SuperChordView::~SuperChordView() {
    stopTimer();
    midiCommandManager.removeListener(this);
    modeScreen->removeListener(this);
    plugin->removeListener(this);
    setLookAndFeel(nullptr);
}

//==============================================================================
void SuperChordView::paint(juce::Graphics &g) {
    // Dark background
    g.fillAll(backgroundColour);

    // Draw chord name in center (the only text element)
    if (currentChordName.isNotEmpty() && chordNameOpacity > 0.01f) {
        g.setColour(textColour.withAlpha(chordNameOpacity));
        g.setFont(juce::Font(56.0f).boldened());

        auto bounds = getLocalBounds();
        g.drawText(currentChordName, bounds.withHeight(100).withY(bounds.getCentreY() - 50),
                   juce::Justification::centred);
    }
}

void SuperChordView::resized() {
    auto bounds = getLocalBounds();

    // All visual components fill the entire area (they overlay each other)
    particleSystem->setBounds(bounds);
    waveformRing->setBounds(bounds);
    visualizer->setBounds(bounds);
    modeScreen->setBounds(bounds);
}

//==============================================================================
void SuperChordView::timerCallback() {
    // Decay encoder activity indicators
    for (int i = 0; i < 4; ++i) {
        if (lastEncoderActivity[i] > 0.0f) {
            lastEncoderActivity[i] -= 0.05f;
            if (lastEncoderActivity[i] < 0.0f)
                lastEncoderActivity[i] = 0.0f;
        }
    }

    // Fade chord name opacity
    if (plugin->isNotePlaying()) {
        chordNameOpacity = juce::jmin(1.0f, chordNameOpacity + 0.1f);
    } else {
        chordNameOpacity = juce::jmax(0.0f, chordNameOpacity - 0.02f);
    }

    // Update particle system with current parameters
    particleSystem->setWarmth(plugin->warmthValue.get());
    particleSystem->setSpace(plugin->spaceValue.get());
    particleSystem->setAttack(plugin->attackValue.get());
    particleSystem->setBloom(plugin->bloomValue.get());

    // Update with encoder activity
    for (int i = 0; i < 4; ++i) {
        particleSystem->setEncoderActivity(i, lastEncoderActivity[i]);
    }

    // Update waveform ring
    waveformRing->setPitchWheelPosition(plugin->getPitchWheelPosition());

    // Update visualizer
    visualizer->update();

    repaint();
}

//==============================================================================
void SuperChordView::encoder1Increased() {
    if (modeScreenVisible) {
        // Blue encoder controls Scale
        modeScreen->nextScale();
    } else {
        // Increase warmth
        float current = plugin->warmthValue.get();
        plugin->warmthParam->setParameter(juce::jmin(1.0f, current + 0.02f),
                                          juce::sendNotification);
        lastEncoderActivity[0] = 1.0f;
        particleSystem->triggerEncoderPulse(0);
    }
}

void SuperChordView::encoder1Decreased() {
    if (modeScreenVisible) {
        // Blue encoder controls Scale
        modeScreen->previousScale();
    } else {
        // Decrease warmth
        float current = plugin->warmthValue.get();
        plugin->warmthParam->setParameter(juce::jmax(0.0f, current - 0.02f),
                                          juce::sendNotification);
        lastEncoderActivity[0] = 1.0f;
        particleSystem->triggerEncoderPulse(0);
    }
}

void SuperChordView::encoder1ButtonReleased() {
    if (modeScreenVisible) {
        // Close mode screen
        modeScreenVisible = false;
        modeScreen->setVisible(false);
    } else {
        // Open mode screen and sync with current plugin state
        modeScreenVisible = true;
        modeScreen->setCurrentScale(plugin->scaleTypeValue.get());
        modeScreen->setCurrentPlayMode(plugin->playModeValue.get());
        modeScreen->setCurrentVoicePreset(plugin->voicePresetValue.get());
        modeScreen->setVisible(true);
    }
}

void SuperChordView::encoder2Increased() {
    if (modeScreenVisible) {
        // Green encoder controls Play Mode
        modeScreen->nextPlayMode();
    } else {
        // Increase space
        float current = plugin->spaceValue.get();
        plugin->spaceParam->setParameter(juce::jmin(1.0f, current + 0.02f),
                                         juce::sendNotification);
        lastEncoderActivity[1] = 1.0f;
        particleSystem->triggerEncoderPulse(1);
    }
}

void SuperChordView::encoder2Decreased() {
    if (modeScreenVisible) {
        // Green encoder controls Play Mode
        modeScreen->previousPlayMode();
    } else {
        // Decrease space
        float current = plugin->spaceValue.get();
        plugin->spaceParam->setParameter(juce::jmax(0.0f, current - 0.02f),
                                         juce::sendNotification);
        lastEncoderActivity[1] = 1.0f;
        particleSystem->triggerEncoderPulse(1);
    }
}

void SuperChordView::encoder3Increased() {
    if (modeScreenVisible) {
        // Encoder 3 does nothing in mode screen (not assigned)
    } else {
        // Increase attack
        float current = plugin->attackValue.get();
        plugin->attackParam->setParameter(juce::jmin(1.0f, current + 0.02f),
                                          juce::sendNotification);
        lastEncoderActivity[2] = 1.0f;
        particleSystem->triggerEncoderPulse(2);
    }
}

void SuperChordView::encoder3Decreased() {
    if (modeScreenVisible) {
        // Encoder 3 does nothing in mode screen (not assigned)
    } else {
        // Decrease attack
        float current = plugin->attackValue.get();
        plugin->attackParam->setParameter(juce::jmax(0.0f, current - 0.02f),
                                          juce::sendNotification);
        lastEncoderActivity[2] = 1.0f;
        particleSystem->triggerEncoderPulse(2);
    }
}

void SuperChordView::encoder4Increased() {
    if (modeScreenVisible) {
        // Red encoder controls Voice Preset
        modeScreen->nextVoicePreset();
    } else {
        // Increase bloom
        float current = plugin->bloomValue.get();
        plugin->bloomParam->setParameter(juce::jmin(1.0f, current + 0.02f),
                                         juce::sendNotification);
        lastEncoderActivity[3] = 1.0f;
        particleSystem->triggerEncoderPulse(3);
    }
}

void SuperChordView::encoder4Decreased() {
    if (modeScreenVisible) {
        // Red encoder controls Voice Preset
        modeScreen->previousVoicePreset();
    } else {
        // Decrease bloom
        float current = plugin->bloomValue.get();
        plugin->bloomParam->setParameter(juce::jmax(0.0f, current - 0.02f),
                                         juce::sendNotification);
        lastEncoderActivity[3] = 1.0f;
        particleSystem->triggerEncoderPulse(3);
    }
}

//==============================================================================
void SuperChordView::chordChanged() {
    currentChordName = plugin->getCurrentChordName();
    chordNameOpacity = 1.0f;

    // Update visualizer with new chord
    auto chordNotes = plugin->getChordEngine().getChordNotes(
        60, plugin->rootNoteValue.get(),
        static_cast<internal_plugins::ChordEngine::ScaleType>(
            plugin->scaleTypeValue.get()),
        plugin->getPitchWheelPosition(),
        static_cast<internal_plugins::ChordEngine::VoicingType>(
            plugin->playSubModeValue.get()));

    visualizer->setChordNotes(chordNotes);
}

void SuperChordView::parameterChanged() {
    updateVisualizationsFromParameters();
}

void SuperChordView::noteTriggered(float velocity) {
    // Trigger note-on animations
    particleSystem->triggerNoteBurst(velocity);
    waveformRing->triggerPulse(velocity);
    visualizer->triggerBloom(velocity);
}

void SuperChordView::updateVisualizationsFromParameters() {
    particleSystem->setWarmth(plugin->warmthValue.get());
    particleSystem->setSpace(plugin->spaceValue.get());
    particleSystem->setAttack(plugin->attackValue.get());
    particleSystem->setBloom(plugin->bloomValue.get());
}

//==============================================================================
// ModeScreen::Listener callbacks - apply settings to plugin
void SuperChordView::scaleTypeChanged(int newType) {
    plugin->scaleTypeValue = newType;
}

void SuperChordView::playModeChanged(int newMode) {
    plugin->playModeValue = newMode;
}

void SuperChordView::voicePresetChanged(int newPreset) {
    plugin->voicePresetValue = newPreset;
}
