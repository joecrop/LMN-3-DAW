#pragma once
#include "AppLookAndFeel.h"
#include "Knobs.h"
#include <app_services/app_services.h>
#include <internal_plugins/internal_plugins.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SuperChordArpeggiatorPage
    : public juce::Component,
      public app_services::MidiCommandManager::Listener,
      public juce::Timer {
  public:
    SuperChordArpeggiatorPage(internal_plugins::SuperChordPlugin *p,
                              app_services::MidiCommandManager &mcm);

    ~SuperChordArpeggiatorPage() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    void timerCallback() override;

    void encoder1Increased() override;
    void encoder1Decreased() override;
    void encoder2Increased() override;
    void encoder2Decreased() override;
    void encoder3Increased() override;
    void encoder3Decreased() override;
    void encoder4Increased() override;
    void encoder4Decreased() override;

  private:
    internal_plugins::SuperChordPlugin *plugin;
    app_services::MidiCommandManager &midiCommandManager;

    juce::Label titleLabel;
    juce::Label arpStatusLabel;
    Knobs pluginKnobs;
    AppLookAndFeel appLookAndFeel;

    void updateKnobValues();
    void drawStepIndicator(juce::Graphics &g, int x, int y, int width,
                           int height);
    juce::String getArpModeName(int value);
    juce::String getArpDirectionName(int value);
    juce::String getArpRateName(int value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChordArpeggiatorPage)
};
