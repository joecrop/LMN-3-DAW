#pragma once
#include "AppLookAndFeel.h"
#include <app_services/app_services.h>
#include <internal_plugins/internal_plugins.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SuperChordView : public juce::TabbedComponent,
                       public app_services::MidiCommandManager::Listener {
  public:
    SuperChordView(internal_plugins::SuperChordPlugin *p,
                   app_services::MidiCommandManager &mcm);

    ~SuperChordView() override;

    void paint(juce::Graphics &g) override;
    void resized() override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;

    void plusButtonReleased() override;
    void minusButtonReleased() override;

  private:
    internal_plugins::SuperChordPlugin *plugin;
    app_services::MidiCommandManager &midiCommandManager;

    juce::String visualizerTabName = "VISUALIZER";
    juce::String soundTabName = "SOUND";
    juce::String modeTabName = "MODE";
    juce::String arpeggiatorTabName = "ARP";

    AppLookAndFeel appLookAndFeel;
    juce::Label pageLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChordView)
};
