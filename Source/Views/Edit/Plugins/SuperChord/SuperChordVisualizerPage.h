#pragma once
#include "AppLookAndFeel.h"
#include <app_services/app_services.h>
#include <internal_plugins/internal_plugins.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SuperChordVisualizerPage
    : public juce::Component,
      public app_services::MidiCommandManager::Listener,
      public juce::Timer {
  public:
    SuperChordVisualizerPage(internal_plugins::SuperChordPlugin *p,
                             app_services::MidiCommandManager &mcm);

    ~SuperChordVisualizerPage() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    void timerCallback() override;

    // Encoder handlers for progression control
    void encoder1Increased() override;
    void encoder1Decreased() override;

  private:
    internal_plugins::SuperChordPlugin *plugin;
    app_services::MidiCommandManager &midiCommandManager;

    AppLookAndFeel appLookAndFeel;

    juce::Label presetNameLabel;
    juce::Label chordNameLabel;
    juce::Label keySignatureLabel;
    juce::Label progressionLabel;

    void drawChordConstellation(juce::Graphics &g, float centerX, float centerY,
                                float radius);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChordVisualizerPage)
};
