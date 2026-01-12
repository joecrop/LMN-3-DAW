#pragma once
#include "AppLookAndFeel.h"
#include "Knobs.h"
#include <app_services/app_services.h>
#include <internal_plugins/internal_plugins.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SuperChordSoundPage
    : public juce::Component,
      public app_services::MidiCommandManager::Listener {
  public:
    SuperChordSoundPage(internal_plugins::SuperChordPlugin *p,
                        app_services::MidiCommandManager &mcm);

    ~SuperChordSoundPage() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    void encoder1Increased() override;
    void encoder1Decreased() override;
    void encoder2Increased() override;
    void encoder2Decreased() override;
    void encoder3Increased() override;
    void encoder3Decreased() override;
    void encoder4Increased() override;
    void encoder4Decreased() override;
    void controlButtonPressed() override;
    void controlButtonReleased() override;

  private:
    internal_plugins::SuperChordPlugin *plugin;
    app_services::MidiCommandManager &midiCommandManager;

    juce::Label titleLabel;
    juce::Label presetNameLabel;
    Knobs pluginKnobs;
    AppLookAndFeel appLookAndFeel;

    int lastPresetIndex = -1;
    bool showSecondaryMacros = false;

    void updateKnobValues();
    void updateMacroLabels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChordSoundPage)
};
