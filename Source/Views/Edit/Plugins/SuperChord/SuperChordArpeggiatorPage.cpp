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

    // Encoder 2: Direction (0-19: Up/Down/Up-Down/Random + 16 patterns)
    pluginKnobs.getKnob(1)->getLabel().setText("Direction",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(1)->getSlider().setRange(0, 19, 1);
    pluginKnobs.getKnob(1)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour2);

    // Encoder 3: Steps (2-16)
    pluginKnobs.getKnob(2)->getLabel().setText("Steps",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(2)->getSlider().setRange(2, 16, 1);
    pluginKnobs.getKnob(2)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour3);

    // Encoder 4: Rate (depends on mode - arp rate or strum rate)
    // Initial setup for arp rate (will be updated based on mode)
    pluginKnobs.getKnob(3)->getLabel().setText("Rate",
                                               juce::dontSendNotification);
    pluginKnobs.getKnob(3)->getSlider().setRange(0, 13, 1);
    pluginKnobs.getKnob(3)->getSlider().setColour(
        juce::Slider::thumbColourId, appLookAndFeel.colour4);

    addAndMakeVisible(pluginKnobs);

    midiCommandManager.addListener(this);

    // Initialize mode tracking and update knobs
    lastArpMode = plugin->getArpModeValue();
    updateKnob4ForMode(lastArpMode);
    updateKnobValues();

    // Start timer for step indicator animation (20 FPS)
    startTimerHz(20);
}

SuperChordArpeggiatorPage::~SuperChordArpeggiatorPage() {
    stopTimer();
    midiCommandManager.removeListener(this);
}

void SuperChordArpeggiatorPage::timerCallback() {
    // Check if mode changed and update knob 4 accordingly
    int currentMode = plugin->getArpModeValue();
    if (currentMode != lastArpMode) {
        lastArpMode = currentMode;
        updateKnob4ForMode(currentMode);
    }
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
    static const char* names[] = {
        "Up",         // 0
        "Down",       // 1
        "Up-Down",    // 2
        "Random",     // 3
        "Swing",      // 4: 1-2-5-4-3
        "Jump",       // 5: 4-7-1-2
        "Stagger",    // 6: 1-3-2-4
        "Double",     // 7: 1-1-2-2
        "Pedal",      // 8: 1-2-1-3-1
        "Skip",       // 9: 1-3-5-7
        "Skip Dn",    // 10: 8-6-4-2
        "Thirds",     // 11: 1-4-2-5
        "Fourths",    // 12: 1-5-2-6
        "Out-In",     // 13: 1-8-2-7
        "In-Out",     // 14: 4-5-3-6
        "Wave",       // 15: 1-2-3-1-2-3-4
        "Accent",     // 16: 1-1-1-2-2-3
        "Pi",         // 17: 3-1-4-1-5-9
        "Fibo",       // 18: 1-1-2-3-5-8
        "Circle"      // 19: 1-4-7-2-5-8
    };
    if (value >= 0 && value < 20)
        return names[value];
    return "Up";
}

juce::String SuperChordArpeggiatorPage::getArpRateName(int value) {
    // Ordered slow to fast
    static const char *rates[] = {
        "2/1",    // 0: 2 whole notes
        "1/1",    // 1: whole note
        "1/1T",   // 2: whole triplet
        "1/2",    // 3: half note
        "1/2T",   // 4: half triplet
        "1/4",    // 5: quarter note
        "1/4T",   // 6: quarter triplet
        "1/8",    // 7: eighth note
        "1/8T",   // 8: eighth triplet
        "1/16",   // 9: sixteenth note
        "1/16T",  // 10: sixteenth triplet
        "1/32",   // 11: thirty-second note
        "1/32T",  // 12: thirty-second triplet
        "1/64"    // 13: sixty-fourth note
    };
    if (value >= 0 && value < 14)
        return rates[value];
    return "1/4";
}

juce::String SuperChordArpeggiatorPage::getStrumRateName(float value) {
    return juce::String(static_cast<int>(value)) + "ms";
}

void SuperChordArpeggiatorPage::updateKnob4ForMode(int arpMode) {
    if (arpMode == 2) {
        // Strum mode - show strum rate in ms
        pluginKnobs.getKnob(3)->getLabel().setText("Strum ms",
                                                   juce::dontSendNotification);
        pluginKnobs.getKnob(3)->getSlider().setRange(10, 200, 1);
    } else {
        // Other modes - show arp rate
        pluginKnobs.getKnob(3)->getLabel().setText("Rate",
                                                   juce::dontSendNotification);
        pluginKnobs.getKnob(3)->getSlider().setRange(0, 13, 1);
    }
}

void SuperChordArpeggiatorPage::updateKnobValues() {
    int arpMode = plugin->getArpModeValue();
    
    pluginKnobs.getKnob(0)->getSlider().setValue(arpMode,
                                                 juce::dontSendNotification);
    pluginKnobs.getKnob(1)->getSlider().setValue(plugin->getArpDirectionValue(),
                                                 juce::dontSendNotification);
    pluginKnobs.getKnob(2)->getSlider().setValue(plugin->getArpStepsValue(),
                                                 juce::dontSendNotification);
    
    // Knob 4 depends on mode
    if (arpMode == 2) {
        // Strum mode - show strum rate
        pluginKnobs.getKnob(3)->getSlider().setValue(plugin->getStrumRateValue(),
                                                     juce::dontSendNotification);
    } else {
        // Other modes - show arp rate
        pluginKnobs.getKnob(3)->getSlider().setValue(plugin->getArpRateValue(),
                                                     juce::dontSendNotification);
    }

    // Update status
    juce::String info = "Mode:" + getArpModeName(arpMode);
    info += " Dir:" + getArpDirectionName(plugin->getArpDirectionValue());
    info += " Steps:" + juce::String(plugin->getArpStepsValue());
    if (arpMode == 2) {
        info += " Strum:" + getStrumRateName(plugin->getStrumRateValue());
    } else {
        info += " Rate:" + getArpRateName(plugin->getArpRateValue());
    }
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
        if (currentValue < 19) {
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
        int arpMode = plugin->getArpModeValue();
        if (arpMode == 2) {
            // Strum mode - adjust strum rate (10-200ms)
            float currentValue = plugin->getStrumRateValue();
            if (currentValue < 200.0f) {
                plugin->strumRateParam->setParameter(
                    currentValue + 5.0f, juce::sendNotification);
                updateKnobValues();
            }
        } else {
            // Other modes - adjust arp rate
            int currentValue = plugin->getArpRateValue();
            if (currentValue < 13) {
                plugin->arpRateParam->setParameter(
                    static_cast<float>(currentValue + 1), juce::sendNotification);
                updateKnobValues();
            }
        }
    }
}

void SuperChordArpeggiatorPage::encoder4Decreased() {
    if (isShowing() && midiCommandManager.getFocusedComponent() == this) {
        int arpMode = plugin->getArpModeValue();
        if (arpMode == 2) {
            // Strum mode - adjust strum rate (10-200ms)
            float currentValue = plugin->getStrumRateValue();
            if (currentValue > 10.0f) {
                plugin->strumRateParam->setParameter(
                    currentValue - 5.0f, juce::sendNotification);
                updateKnobValues();
            }
        } else {
            // Other modes - adjust arp rate
            int currentValue = plugin->getArpRateValue();
            if (currentValue > 0) {
                plugin->arpRateParam->setParameter(
                    static_cast<float>(currentValue - 1), juce::sendNotification);
                updateKnobValues();
            }
        }
    }
}
