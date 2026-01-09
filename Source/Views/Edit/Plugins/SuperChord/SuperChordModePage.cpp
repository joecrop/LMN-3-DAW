#include "SuperChordModePage.h"

SuperChordModePage::SuperChordModePage(internal_plugins::SuperChordPlugin *p,
                                       app_services::MidiCommandManager &mcm)
    : plugin(p), midiCommandManager(mcm), pluginKnobs(mcm, 4) {
    // Title label
    titleLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                  getHeight() * .1, juce::Font::bold));
    titleLabel.setText("SuperChord: Mode", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, appLookAndFeel.whiteColour);
    addAndMakeVisible(titleLabel);

    // Mode summary label
    modeSummaryLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), 12, juce::Font::plain));
    modeSummaryLabel.setJustificationType(juce::Justification::centred);
    modeSummaryLabel.setColour(juce::Label::textColourId,
                               juce::Colours::lightgrey);
    addAndMakeVisible(modeSummaryLabel);

    // Configure knobs
    // Encoder 1: Voicing (0-2: Close/Open/Drop-2)
    pluginKnobs.getKnob(0)->getLabel().setText("Voicing",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(0)->getSlider().setRange(0, 2, 1);
    pluginKnobs.getKnob(0)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour1);

    // Encoder 2: Octave (-1 to +1)
    pluginKnobs.getKnob(1)->getLabel().setText("Octave",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(1)->getSlider().setRange(-1, 1, 1);
    pluginKnobs.getKnob(1)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour2);

    // Encoder 3: Key (0-11: C-B)
    pluginKnobs.getKnob(2)->getLabel().setText("Key", juce::dontSendNotification);
    pluginKnobs.getKnob(2)->getSlider().setRange(0, 11, 1);
    pluginKnobs.getKnob(2)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour3);

    // Encoder 4: Extension (0-2: Triad/7th/9th)
    pluginKnobs.getKnob(3)->getLabel().setText("Extension",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(3)->getSlider().setRange(0, 2, 1);
    pluginKnobs.getKnob(3)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour4);

    addAndMakeVisible(pluginKnobs);

    midiCommandManager.addListener(this);

    updateKnobValues();
}

SuperChordModePage::~SuperChordModePage() {
    midiCommandManager.removeListener(this);
}

void SuperChordModePage::paint(juce::Graphics &g) {
    // Background handled by parent
}

void SuperChordModePage::resized() {
    float titleFontSize = juce::jmax(getHeight() * 0.1f, 20.0f);
    float summaryFontSize = juce::jmax(getHeight() * 0.06f, 14.0f);

    titleLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                  titleFontSize, juce::Font::bold));
    titleLabel.setBounds(0, getHeight() * .05, getWidth(), getHeight() * .1);

    int knobWidth = getWidth() / 8;
    int knobHeight = getHeight() / 3;
    int knobSpacing = knobWidth;
    int width = (4 * knobWidth) + (3 * knobSpacing);
    int height = knobHeight;
    int startX = (getWidth() / 2) - (width / 2);
    int startY = (getHeight() / 2) - (knobHeight / 2);
    juce::Rectangle<int> bounds(startX, startY, width, height);
    pluginKnobs.setGridSpacing(knobSpacing);
    pluginKnobs.setBounds(bounds);

    modeSummaryLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                        summaryFontSize, juce::Font::plain));
    modeSummaryLabel.setBounds(0, getHeight() * .85, getWidth(),
                               getHeight() * .1);
}

juce::String SuperChordModePage::getVoicingName(int value) {
    switch (value) {
    case 0:
        return "Close";
    case 1:
        return "Open";
    case 2:
        return "Drop-2";
    default:
        return "Close";
    }
}

juce::String SuperChordModePage::getKeyName(int value) {
    static const char *keys[] = {"C",  "C#", "D",  "D#", "E",  "F",
                                 "F#", "G",  "G#", "A",  "A#", "B"};
    if (value >= 0 && value < 12)
        return keys[value];
    return "C";
}

juce::String SuperChordModePage::getExtensionName(int value) {
    switch (value) {
    case 0:
        return "Triad";
    case 1:
        return "7th";
    case 2:
        return "9th";
    default:
        return "Triad";
    }
}

void SuperChordModePage::updateKnobValues() {
    pluginKnobs.getKnob(0)->getSlider().setValue(plugin->getVoicingValue(),
                                                 juce::dontSendNotification);
    pluginKnobs.getKnob(1)->getSlider().setValue(plugin->getOctaveValue(),
                                                 juce::dontSendNotification);
    pluginKnobs.getKnob(2)->getSlider().setValue(plugin->getKeyValue(),
                                                 juce::dontSendNotification);
    pluginKnobs.getKnob(3)->getSlider().setValue(plugin->getExtensionValue(),
                                                 juce::dontSendNotification);

    // Update summary
    juce::String info = juce::String("Voicing:") + getVoicingName(plugin->getVoicingValue());
    int octave = plugin->getOctaveValue();
    info += juce::String(" Oct:") + (octave >= 0 ? "+" : "") + juce::String(octave);
    info += juce::String(" Key:") + getKeyName(plugin->getKeyValue());
    info += juce::String(" Ext:") + getExtensionName(plugin->getExtensionValue());
    modeSummaryLabel.setText(info, juce::dontSendNotification);
}

void SuperChordModePage::encoder1Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getVoicingValue();
        if (currentValue < 2) {
            plugin->voicingParam->setParameter(
                static_cast<float>(currentValue + 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordModePage::encoder1Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getVoicingValue();
        if (currentValue > 0) {
            plugin->voicingParam->setParameter(
                static_cast<float>(currentValue - 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordModePage::encoder2Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getOctaveValue();
        if (currentValue < 1) {
            plugin->octaveParam->setParameter(
                static_cast<float>(currentValue + 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordModePage::encoder2Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getOctaveValue();
        if (currentValue > -1) {
            plugin->octaveParam->setParameter(
                static_cast<float>(currentValue - 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordModePage::encoder3Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getKeyValue();
        if (currentValue < 11) {
            plugin->keyParam->setParameter(static_cast<float>(currentValue + 1),
                                           juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordModePage::encoder3Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getKeyValue();
        if (currentValue > 0) {
            plugin->keyParam->setParameter(static_cast<float>(currentValue - 1),
                                           juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordModePage::encoder4Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getExtensionValue();
        if (currentValue < 2) {
            plugin->extensionParam->setParameter(
                static_cast<float>(currentValue + 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordModePage::encoder4Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getExtensionValue();
        if (currentValue > 0) {
            plugin->extensionParam->setParameter(
                static_cast<float>(currentValue - 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}
