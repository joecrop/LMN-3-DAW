#pragma once

#include "ChordEngine.h"
#include "SuperChordSound.h"
#include "SuperChordVoice.h"
#include "VoicePresets.h"

namespace internal_plugins {

/**
 * SuperChord - A beautiful chord synthesis instrument.
 *
 * Transforms single MIDI notes into rich diatonic chords with
 * pitch wheel modulation for smooth chord quality morphing.
 * Features pre-designed voice presets controlled by 4 macro
 * parameters: Warmth, Space, Attack, and Bloom.
 */
class SuperChordPlugin : public tracktion::Plugin,
                         private juce::AsyncUpdater {
  public:
    explicit SuperChordPlugin(tracktion::PluginCreationInfo info);
    ~SuperChordPlugin() override;

    //==========================================================================
    // Plugin identification
    static const char *getPluginName() { return NEEDS_TRANS("SuperChord"); }
    static const char *xmlTypeName;

    juce::String getName() override { return TRANS("SuperChord"); }
    juce::String getPluginType() override { return xmlTypeName; }
    juce::String getShortName(int) override { return "SprChd"; }
    juce::String getSelectableDescription() override {
        return TRANS("SuperChord");
    }

    //==========================================================================
    // Plugin properties
    bool isSynth() override { return true; }
    bool needsConstantBufferSize() override { return false; }
    int getNumOutputChannelsGivenInputs(int numInputChannels) override {
        return juce::jmin(numInputChannels, 2);
    }

    //==========================================================================
    // Audio processing
    void initialise(const tracktion::PluginInitialisationInfo &) override;
    void deinitialise() override;
    void applyToBuffer(const tracktion::PluginRenderContext &) override;
    void reset() override;

    //==========================================================================
    // MIDI handling
    bool takesMidiInput() override { return true; }
    bool takesAudioInput() override { return false; }
    bool producesAudioWhenNoAudioInput() override { return true; }
    double getTailLength() const override;

    //==========================================================================
    // State management
    void restorePluginStateFromValueTree(const juce::ValueTree &) override;

    //==========================================================================
    // Macro parameters (0.0 - 1.0)
    juce::CachedValue<float> warmthValue;
    juce::CachedValue<float> spaceValue;
    juce::CachedValue<float> attackValue;
    juce::CachedValue<float> bloomValue;

    tracktion::AutomatableParameter::Ptr warmthParam;
    tracktion::AutomatableParameter::Ptr spaceParam;
    tracktion::AutomatableParameter::Ptr attackParam;
    tracktion::AutomatableParameter::Ptr bloomParam;

    //==========================================================================
    // Musical settings
    juce::CachedValue<int> rootNoteValue;  // 0-11 (C=0, C#=1, etc.)
    juce::CachedValue<int> scaleTypeValue; // Scale enum index
    juce::CachedValue<int> voicePresetValue;
    juce::CachedValue<int> playModeValue;    // Chord, Strum, Arp, Pad
    juce::CachedValue<int> playSubModeValue; // Sub-option for each mode

    //==========================================================================
    // Engine access
    ChordEngine &getChordEngine() { return chordEngine; }
    const ChordEngine &getChordEngine() const { return chordEngine; }
    VoicePresets &getVoicePresets() { return voicePresets; }

    //==========================================================================
    // Current state for UI
    juce::String getCurrentChordName() const;
    float getPitchWheelPosition() const { return currentPitchWheel; }
    float getLastNoteVelocity() const { return lastNoteVelocity; }
    bool isNotePlaying() const { return notesPlaying > 0; }

    //==========================================================================
    // Listener for UI updates
    class Listener {
      public:
        virtual ~Listener() = default;
        virtual void chordChanged() {}
        virtual void parameterChanged() {}
        virtual void noteTriggered(float velocity) {}
    };

    void addListener(Listener *l) { listeners.add(l); }
    void removeListener(Listener *l) { listeners.remove(l); }

  private:
    void handleAsyncUpdate() override;
    void processNoteOn(int noteNumber, float velocity);
    void processNoteOff(int noteNumber);
    void processPitchWheel(int wheelValue);
    void updateSynthParameters();

    juce::Synthesiser synthesiser;
    ChordEngine chordEngine;
    VoicePresets voicePresets;

    int blockSize = 512;
    float currentPitchWheel = 0.5f; // 0.0-1.0, center = 0.5
    float lastNoteVelocity = 0.0f;
    int notesPlaying = 0;

    // Track which notes are generating chords
    std::map<int, std::vector<int>> activeChordNotes;

    juce::ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChordPlugin)
};

} // namespace internal_plugins
