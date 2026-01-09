#pragma once
#include "AppLookAndFeel.h"
#include "Knobs.h"
#include <app_services/app_services.h>
#include <internal_plugins/internal_plugins.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SuperChordModePage : public juce::Component,
                           public app_services::MidiCommandManager::Listener {
  public:
    SuperChordModePage(internal_plugins::SuperChordPlugin *p,
                       app_services::MidiCommandManager &mcm);

    ~SuperChordModePage() override;

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

  private:
    internal_plugins::SuperChordPlugin *plugin;
    app_services::MidiCommandManager &midiCommandManager;

    juce::Label titleLabel;
    juce::Label modeSummaryLabel;
    Knobs pluginKnobs;
    AppLookAndFeel appLookAndFeel;

    void updateKnobValues();
    juce::String getVoicingName(int value);
    juce::String getKeyName(int value);
    juce::String getExtensionName(int value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChordModePage)
};
