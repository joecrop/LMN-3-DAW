#include "SuperChordSoundPage.h"

SuperChordSoundPage::SuperChordSoundPage(
    internal_plugins::SuperChordPlugin *p,
    app_services::MidiCommandManager &mcm)
    : plugin(p), midiCommandManager(mcm), pluginKnobs(mcm, 4) {
    // Title label
    titleLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                  getHeight() * .1, juce::Font::bold));
    titleLabel.setText("SuperChord: Sound", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, appLookAndFeel.whiteColour);
    addAndMakeVisible(titleLabel);

    // Preset name label
    presetNameLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), 18, juce::Font::bold));
    presetNameLabel.setJustificationType(juce::Justification::centred);
    presetNameLabel.setColour(juce::Label::textColourId,
                              appLookAndFeel.blueColour);
    addAndMakeVisible(presetNameLabel);

    // Configure knobs
    // Encoder 1: Voice Preset (1-64)
    pluginKnobs.getKnob(0)->getLabel().setText("Voice",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(0)->getSlider().setRange(1, 64, 1);
    pluginKnobs.getKnob(0)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour1);

    // Encoder 2: Macro 1 (0-100%)
    pluginKnobs.getKnob(1)->getLabel().setText("Macro 1",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(1)->getSlider().setRange(0, 100, 1);
    pluginKnobs.getKnob(1)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour2);

    // Encoder 3: Macro 2 (0-100%)
    pluginKnobs.getKnob(2)->getLabel().setText("Macro 2",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(2)->getSlider().setRange(0, 100, 1);
    pluginKnobs.getKnob(2)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour3);

    // Encoder 4: Macro 3 (0-100%)
    pluginKnobs.getKnob(3)->getLabel().setText("Macro 3",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(3)->getSlider().setRange(0, 100, 1);
    pluginKnobs.getKnob(3)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour4);

    addAndMakeVisible(pluginKnobs);

    midiCommandManager.addListener(this);

    updateKnobValues();
    updateMacroLabels();
}

SuperChordSoundPage::~SuperChordSoundPage() {
    midiCommandManager.removeListener(this);
}

void SuperChordSoundPage::paint(juce::Graphics &g) {
    // Background handled by parent
}

void SuperChordSoundPage::resized() {
    float titleFontSize = juce::jmax(getHeight() * 0.1f, 20.0f);
    float presetFontSize = juce::jmax(getHeight() * 0.08f, 18.0f);

    titleLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                  titleFontSize, juce::Font::bold));
    titleLabel.setBounds(0, getHeight() * .05, getWidth(), getHeight() * .1);

    presetNameLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                       presetFontSize, juce::Font::bold));
    presetNameLabel.setBounds(0, getHeight() * .15, getWidth(),
                              getHeight() * .08);

    int knobWidth = getWidth() / 8;
    int knobHeight = getHeight() / 3;
    int knobSpacing = knobWidth;
    int width = (4 * knobWidth) + (3 * knobSpacing);
    int height = knobHeight;
    int startX = (getWidth() / 2) - (width / 2);
    int startY = (getHeight() / 2) - (knobHeight / 2) + 10;
    juce::Rectangle<int> bounds(startX, startY, width, height);
    pluginKnobs.setGridSpacing(knobSpacing);
    pluginKnobs.setBounds(bounds);
}

void SuperChordSoundPage::updateKnobValues() {
    int presetIndex = plugin->getCurrentVoicePresetIndex();
    pluginKnobs.getKnob(0)->getSlider().setValue(presetIndex + 1,
                                                 juce::dontSendNotification);
    pluginKnobs.getKnob(1)->getSlider().setValue(
        plugin->getMacro1Value() * 100.0, juce::dontSendNotification);
    pluginKnobs.getKnob(2)->getSlider().setValue(
        plugin->getMacro2Value() * 100.0, juce::dontSendNotification);
    pluginKnobs.getKnob(3)->getSlider().setValue(
        plugin->getMacro3Value() * 100.0, juce::dontSendNotification);

    presetNameLabel.setText(plugin->getCurrentPresetName(),
                            juce::dontSendNotification);

    // Check if preset changed and update macro labels
    if (presetIndex != lastPresetIndex) {
        lastPresetIndex = presetIndex;
        updateMacroLabels();
    }
}

void SuperChordSoundPage::updateMacroLabels() {
    int presetIndex = plugin->getCurrentVoicePresetIndex();
    const internal_plugins::VoicePreset &preset =
        internal_plugins::VoicePresets::getPreset(presetIndex);

    pluginKnobs.getKnob(1)->getLabel().setText(preset.macros[0].name,
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(2)->getLabel().setText(preset.macros[1].name,
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(3)->getLabel().setText(preset.macros[2].name,
                                               juce::dontSendNotification);
}

void SuperChordSoundPage::encoder1Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue =
            static_cast<int>(pluginKnobs.getKnob(0)->getSlider().getValue());
        if (currentValue < 64) {
            plugin->voicePresetParam->setParameter(
                static_cast<float>(currentValue + 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordSoundPage::encoder1Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue =
            static_cast<int>(pluginKnobs.getKnob(0)->getSlider().getValue());
        if (currentValue > 1) {
            plugin->voicePresetParam->setParameter(
                static_cast<float>(currentValue - 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordSoundPage::encoder2Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        float currentValue = plugin->getMacro1Value();
        float newValue = juce::jmin(1.0f, currentValue + 0.05f);
        plugin->macro1Param->setParameter(newValue, juce::sendNotification);
        updateKnobValues();
    }
}

void SuperChordSoundPage::encoder2Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        float currentValue = plugin->getMacro1Value();
        float newValue = juce::jmax(0.0f, currentValue - 0.05f);
        plugin->macro1Param->setParameter(newValue, juce::sendNotification);
        updateKnobValues();
    }
}

void SuperChordSoundPage::encoder3Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        float currentValue = plugin->getMacro2Value();
        float newValue = juce::jmin(1.0f, currentValue + 0.05f);
        plugin->macro2Param->setParameter(newValue, juce::sendNotification);
        updateKnobValues();
    }
}

void SuperChordSoundPage::encoder3Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        float currentValue = plugin->getMacro2Value();
        float newValue = juce::jmax(0.0f, currentValue - 0.05f);
        plugin->macro2Param->setParameter(newValue, juce::sendNotification);
        updateKnobValues();
    }
}

void SuperChordSoundPage::encoder4Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        float currentValue = plugin->getMacro3Value();
        float newValue = juce::jmin(1.0f, currentValue + 0.05f);
        plugin->macro3Param->setParameter(newValue, juce::sendNotification);
        updateKnobValues();
    }
}

void SuperChordSoundPage::encoder4Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        float currentValue = plugin->getMacro3Value();
        float newValue = juce::jmax(0.0f, currentValue - 0.05f);
        plugin->macro3Param->setParameter(newValue, juce::sendNotification);
        updateKnobValues();
    }
}
