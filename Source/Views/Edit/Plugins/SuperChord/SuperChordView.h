#pragma once
#include "AppLookAndFeel.h"
#include "ModeScreen.h"
#include "ParticleSystem.h"
#include "Visualizer.h"
#include "WaveformRing.h"
#include <app_services/app_services.h>
#include <app_view_models/app_view_models.h>
#include <internal_plugins/internal_plugins.h>
#include <juce_gui_basics/juce_gui_basics.h>

/**
 * SuperChordView - Abstract visual interface for the SuperChord plugin.
 *
 * Features a fully abstract UI with no text labels or knobs. Users discover
 * functionality through visual feedback from the particle system, waveform
 * ring, and chord constellation. Only the chord name is displayed as text.
 *
 * Encoder colors:
 * - Encoder 1 (Warmth): Blue #458588
 * - Encoder 2 (Space): Green #689d6a
 * - Encoder 3 (Attack): White #f9f5d7
 * - Encoder 4 (Bloom): Red #cc241d
 */
class SuperChordView : public juce::Component,
                       public app_services::MidiCommandManager::Listener,
                       public internal_plugins::SuperChordPlugin::Listener,
                       public ModeScreen::Listener,
                       private juce::Timer {
  public:
    SuperChordView(internal_plugins::SuperChordPlugin *plugin,
                   app_services::MidiCommandManager &mcm);
    ~SuperChordView() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    //==========================================================================
    // MidiCommandManager::Listener
    void encoder1Increased() override;
    void encoder1Decreased() override;
    void encoder1ButtonReleased() override;

    void encoder2Increased() override;
    void encoder2Decreased() override;

    void encoder3Increased() override;
    void encoder3Decreased() override;

    void encoder4Increased() override;
    void encoder4Decreased() override;

    //==========================================================================
    // SuperChordPlugin::Listener
    void chordChanged() override;
    void parameterChanged() override;
    void noteTriggered(float velocity) override;

    //==========================================================================
    // ModeScreen::Listener
    void scaleTypeChanged(int newType) override;
    void playModeChanged(int newMode) override;
    void voicePresetChanged(int newPreset) override;

  private:
    void timerCallback() override;
    void updateVisualizationsFromParameters();

    internal_plugins::SuperChordPlugin *plugin;
    app_services::MidiCommandManager &midiCommandManager;

    // Visual components
    std::unique_ptr<ParticleSystem> particleSystem;
    std::unique_ptr<WaveformRing> waveformRing;
    std::unique_ptr<Visualizer> visualizer;
    std::unique_ptr<ModeScreen> modeScreen;

    // State
    bool modeScreenVisible = false;
    float lastEncoderActivity[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    // Colors matching encoder LEDs
    juce::Colour encoder1Color{0xff458588}; // Blue - Warmth
    juce::Colour encoder2Color{0xff689d6a}; // Green - Space
    juce::Colour encoder3Color{0xfff9f5d7}; // White - Attack
    juce::Colour encoder4Color{0xffcc241d}; // Red - Bloom
    juce::Colour backgroundColour{0xff1d2021};
    juce::Colour textColour{0xfff9f5d7};

    // Chord name display
    juce::String currentChordName;
    float chordNameOpacity = 0.0f;

    AppLookAndFeel appLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChordView)
};
