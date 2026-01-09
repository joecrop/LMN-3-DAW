#include "SuperChordArpeggiatorPage.h"

SuperChordArpeggiatorPage::SuperChordArpeggiatorPage(
    internal_plugins::SuperChordPlugin *p,
    app_services::MidiCommandManager &mcm)
    : plugin(p), midiCommandManager(mcm), pluginKnobs(mcm, 4) {
    // Title label
    titleLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                  getHeight() * .1, juce::Font::bold));
    titleLabel.setText("SuperChord: Arpeggiator", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, appLookAndFeel.whiteColour);
    addAndMakeVisible(titleLabel);

    // Arp status label
    arpStatusLabel.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), 12, juce::Font::plain));
    arpStatusLabel.setJustificationType(juce::Justification::centred);
    arpStatusLabel.setColour(juce::Label::textColourId,
                             juce::Colours::lightgrey);
    addAndMakeVisible(arpStatusLabel);

    // Configure knobs
    // Encoder 1: Arp Mode (0-3: Off/Arpeggiate/Strum/Latch)
    pluginKnobs.getKnob(0)->getLabel().setText("Mode",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(0)->getSlider().setRange(0, 3, 1);
    pluginKnobs.getKnob(0)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour1);

    // Encoder 2: Direction (0-3: Up/Down/Up-Down/Random)
    pluginKnobs.getKnob(1)->getLabel().setText("Direction",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(1)->getSlider().setRange(0, 3, 1);
    pluginKnobs.getKnob(1)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour2);

    // Encoder 3: Steps (2-16)
    pluginKnobs.getKnob(2)->getLabel().setText("Steps",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(2)->getSlider().setRange(2, 16, 1);
    pluginKnobs.getKnob(2)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour3);

    // Encoder 4: Rate (0-6: 1/32 to 2/1)
    pluginKnobs.getKnob(3)->getLabel().setText("Rate",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(3)->getSlider().setRange(0, 6, 1);
    pluginKnobs.getKnob(3)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour4);

    addAndMakeVisible(pluginKnobs);

    midiCommandManager.addListener(this);

    updateKnobValues();

    // Start timer for step indicator animation (20 FPS)
    startTimerHz(20);
}

SuperChordArpeggiatorPage::~SuperChordArpeggiatorPage() {
    stopTimer();
    midiCommandManager.removeListener(this);
}

void SuperChordArpeggiatorPage::timerCallback() {
    updateKnobValues();
    repaint();
}

void SuperChordArpeggiatorPage::paint(juce::Graphics &g) {
    // Draw step indicator at bottom
    int indicatorWidth = getWidth() * 0.6;
    int indicatorX = (getWidth() - indicatorWidth) / 2;
    int indicatorY = getHeight() * 0.75;
    drawStepIndicator(g, indicatorX, indicatorY, indicatorWidth, 20);
}

void SuperChordArpeggiatorPage::resized() {
    float titleFontSize = juce::jmax(getHeight() * 0.1f, 20.0f);
    float statusFontSize = juce::jmax(getHeight() * 0.06f, 14.0f);

    titleLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                  titleFontSize, juce::Font::bold));
    titleLabel.setBounds(0, getHeight() * .05, getWidth(), getHeight() * .1);

    int knobWidth = getWidth() / 8;
    int knobHeight = getHeight() / 3;
    int knobSpacing = knobWidth;
    int width = (4 * knobWidth) + (3 * knobSpacing);
    int height = knobHeight;
    int startX = (getWidth() / 2) - (width / 2);
    int startY = (getHeight() / 2) - (knobHeight / 2) - 20;
    juce::Rectangle<int> bounds(startX, startY, width, height);
    pluginKnobs.setGridSpacing(knobSpacing);
    pluginKnobs.setBounds(bounds);

    arpStatusLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                      statusFontSize, juce::Font::plain));
    arpStatusLabel.setBounds(0, getHeight() * .65, getWidth(),
                             getHeight() * .08);
}

juce::String SuperChordArpeggiatorPage::getArpModeName(int value) {
    switch (value) {
    case 0:
        return "Off";
    case 1:
        return "Arpeggiate";
    case 2:
        return "Strum";
    case 3:
        return "Latch";
    default:
        return "Off";
    }
}

juce::String SuperChordArpeggiatorPage::getArpDirectionName(int value) {
    switch (value) {
    case 0:
        return "Up";
    case 1:
        return "Down";
    case 2:
        return "Up-Down";
    case 3:
        return "Random";
    default:
        return "Up";
    }
}

juce::String SuperChordArpeggiatorPage::getArpRateName(int value) {
    static const char *rates[] = {"1/32", "1/16", "1/8", "1/4",
                                  "1/2",  "1/1",  "2/1"};
    if (value >= 0 && value < 7)
        return rates[value];
    return "1/8";
}

void SuperChordArpeggiatorPage::updateKnobValues() {
    pluginKnobs.getKnob(0)->getSlider().setValue(plugin->getArpModeValue(),
                                                 juce::dontSendNotification);
    pluginKnobs.getKnob(1)->getSlider().setValue(plugin->getArpDirectionValue(),
                                                 juce::dontSendNotification);
    pluginKnobs.getKnob(2)->getSlider().setValue(plugin->getArpStepsValue(),
                                                 juce::dontSendNotification);
    pluginKnobs.getKnob(3)->getSlider().setValue(plugin->getArpRateValue(),
                                                 juce::dontSendNotification);

    // Update status
    juce::String info = "Mode:" + getArpModeName(plugin->getArpModeValue());
    info += " Dir:" + getArpDirectionName(plugin->getArpDirectionValue());
    info += " Steps:" + juce::String(plugin->getArpStepsValue());
    info += " Rate:" + getArpRateName(plugin->getArpRateValue());
    arpStatusLabel.setText(info, juce::dontSendNotification);
}

void SuperChordArpeggiatorPage::drawStepIndicator(juce::Graphics &g, int x,
                                                  int y, int width,
                                                  int height) {
    int numSteps = plugin->getArpStepsValue();
    int currentStep = plugin->getCurrentArpStep();
    int arpMode = plugin->getArpModeValue();

    // Only show animation if arpeggiator is active
    bool isActive = (arpMode == 1); // Arpeggiate mode

    float dotSpacing = static_cast<float>(width) / numSteps;
    float dotRadius = juce::jmin(4.0f, dotSpacing * 0.3f);

    for (int i = 0; i < numSteps; i++) {
        float dotX = x + (i + 0.5f) * dotSpacing;
        float dotY = y + height / 2.0f;

        if (isActive && i == currentStep) {
            // Active step - larger cyan dot
            g.setColour(appLookAndFeel.blueColour);
            g.fillEllipse(dotX - dotRadius * 1.5f, dotY - dotRadius * 1.5f,
                          dotRadius * 3.0f, dotRadius * 3.0f);
        } else {
            // Inactive step - grey dot
            g.setColour(juce::Colours::grey);
            g.fillEllipse(dotX - dotRadius, dotY - dotRadius, dotRadius * 2.0f,
                          dotRadius * 2.0f);
        }
    }

    // Draw step numbers below
    g.setColour(juce::Colours::darkgrey);
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 8,
                         juce::Font::plain));
    g.drawText("1", x, y + height, 20, 12, juce::Justification::centred);
    g.drawText(juce::String(numSteps), x + width - 20, y + height, 20, 12,
               juce::Justification::centred);
}

void SuperChordArpeggiatorPage::encoder1Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getArpModeValue();
        if (currentValue < 3) {
            plugin->arpModeParam->setParameter(
                static_cast<float>(currentValue + 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordArpeggiatorPage::encoder1Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getArpModeValue();
        if (currentValue > 0) {
            plugin->arpModeParam->setParameter(
                static_cast<float>(currentValue - 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordArpeggiatorPage::encoder2Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getArpDirectionValue();
        if (currentValue < 3) {
            plugin->arpDirectionParam->setParameter(
                static_cast<float>(currentValue + 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordArpeggiatorPage::encoder2Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getArpDirectionValue();
        if (currentValue > 0) {
            plugin->arpDirectionParam->setParameter(
                static_cast<float>(currentValue - 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordArpeggiatorPage::encoder3Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getArpStepsValue();
        if (currentValue < 16) {
            plugin->arpStepsParam->setParameter(
                static_cast<float>(currentValue + 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordArpeggiatorPage::encoder3Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getArpStepsValue();
        if (currentValue > 2) {
            plugin->arpStepsParam->setParameter(
                static_cast<float>(currentValue - 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordArpeggiatorPage::encoder4Increased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getArpRateValue();
        if (currentValue < 6) {
            plugin->arpRateParam->setParameter(
                static_cast<float>(currentValue + 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}

void SuperChordArpeggiatorPage::encoder4Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int currentValue = plugin->getArpRateValue();
        if (currentValue > 0) {
            plugin->arpRateParam->setParameter(
                static_cast<float>(currentValue - 1), juce::sendNotification);
            updateKnobValues();
        }
    }
}
