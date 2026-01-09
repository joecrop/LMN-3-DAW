#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <tracktion_engine/tracktion_engine.h>

namespace internal_plugins {

// Forward declarations
class SuperChordVoice;
class SuperChordSound;
class ChordEngine;
struct VoicePreset;

/**
 * SuperChord Plugin - A chord synthesizer plugin inspired by the Hichord pocket
 * chord synthesizer. Implements diatonic chord generation (I-VII Nashville
 * Number System), 16 voice presets with macro controls, and a 4-page tabbed UI.
 */
class SuperChordPlugin : public tracktion::Plugin {
  public:
    explicit SuperChordPlugin(tracktion::PluginCreationInfo info);
    ~SuperChordPlugin() override;

    static const char *getPluginName() { return NEEDS_TRANS("SuperChord"); }
    static const char *xmlTypeName;

    juce::String getName() override { return TRANS("SuperChord"); }
    juce::String getPluginType() override { return xmlTypeName; }
    juce::String getShortName(int) override { return "SprChrd"; }
    juce::String getSelectableDescription() override {
        return TRANS("SuperChord Synthesizer");
    }

    bool needsConstantBufferSize() override { return false; }
    bool producesAudioWhenNoAudioInput() override { return true; }
    bool isSynth() override { return true; }
    bool takesMidiInput() override { return true; }
    int getNumOutputChannelsGivenInputs(int numInputChannels) override {
        return juce::jmax(2, numInputChannels);
    }

    void initialise(const tracktion::PluginInitialisationInfo &) override;
    void deinitialise() override;
    void reset() override;
    void applyToBuffer(const tracktion::PluginRenderContext &fc) override;

    void restorePluginStateFromValueTree(
        const juce::ValueTree &v) override;

    // Parameter accessors
    int getCurrentVoicePresetIndex() const;
    float getMacro1Value() const;
    float getMacro2Value() const;
    float getMacro3Value() const;
    int getVoicingValue() const;
    int getOctaveValue() const;
    int getKeyValue() const;
    int getExtensionValue() const;
    int getArpModeValue() const;
    int getArpDirectionValue() const;
    int getArpStepsValue() const;
    int getArpRateValue() const;

    // State accessors for visualizer
    int getCurrentChordDegree() const { return currentChordDegree; }
    juce::String getCurrentChordName() const { return currentChordName; }
    int getCurrentArpStep() const { return currentArpStep; }
    float getPitchWheelValue() const { return pitchWheelValue; }

    juce::String getKeySignature() const {
        static const char *keys[] = {"C",  "C#", "D",  "D#", "E",  "F",
                                     "F#", "G",  "G#", "A",  "A#", "B"};
        return juce::String(keys[getKeyValue()]) + " Major";
    }

    juce::String getCurrentPresetName() const;

    // Parameter pointers for external binding
    tracktion::AutomatableParameter::Ptr voicePresetParam;
    tracktion::AutomatableParameter::Ptr macro1Param;
    tracktion::AutomatableParameter::Ptr macro2Param;
    tracktion::AutomatableParameter::Ptr macro3Param;
    tracktion::AutomatableParameter::Ptr voicingParam;
    tracktion::AutomatableParameter::Ptr octaveParam;
    tracktion::AutomatableParameter::Ptr keyParam;
    tracktion::AutomatableParameter::Ptr extensionParam;
    tracktion::AutomatableParameter::Ptr arpModeParam;
    tracktion::AutomatableParameter::Ptr arpDirectionParam;
    tracktion::AutomatableParameter::Ptr arpStepsParam;
    tracktion::AutomatableParameter::Ptr arpRateParam;

  private:
    // Cached values for state persistence
    juce::CachedValue<float> voicePresetValue;
    juce::CachedValue<float> macro1Value;
    juce::CachedValue<float> macro2Value;
    juce::CachedValue<float> macro3Value;
    juce::CachedValue<float> voicingValue;
    juce::CachedValue<float> octaveValue;
    juce::CachedValue<float> keyValue;
    juce::CachedValue<float> extensionValue;
    juce::CachedValue<float> arpModeValue;
    juce::CachedValue<float> arpDirectionValue;
    juce::CachedValue<float> arpStepsValue;
    juce::CachedValue<float> arpRateValue;

    // Synthesizer for polyphony
    juce::Synthesiser synthesiser;

    // Arpeggiator state
    int currentArpStep = 0;
    double nextArpTriggerTime = 0.0;
    double arpSampleCounter = 0.0;
    bool arpDirectionUp = true;
    juce::Array<int> activeChordNotes;
    juce::Array<int> latchedNotes;

    // Visualizer state
    int currentChordDegree = -1;
    juce::String currentChordName;
    float pitchWheelValue = 0.0f;

    // Chord engine
    std::unique_ptr<ChordEngine> chordEngine;

    void processNoteOn(int midiNote, float velocity, double currentPPQ,
                       double bpm, int samplePosition);
    void processNoteOff(int midiNote, float velocity);
    void processPitchWheel(int rawValue);
    void triggerArpStep(double currentPPQ, double bpm, int samplePosition);
    double calculateArpInterval(double bpm) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChordPlugin)
};

} // namespace internal_plugins
