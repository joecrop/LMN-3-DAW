#pragma once

#include "VoicePresets.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace internal_plugins {

/**
 * SuperChordVoice - SynthesiserVoice for SuperChord plugin.
 * Implements multi-oscillator synthesis with ADSR envelopes,
 * filters, and LFO modulation.
 */
class SuperChordVoice : public juce::SynthesiserVoice {
  public:
    SuperChordVoice();
    ~SuperChordVoice() override = default;

    bool canPlaySound(juce::SynthesiserSound *sound) override;

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound *sound,
                   int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                         int startSample, int numSamples) override;

    void setCurrentPlaybackSampleRate(double newRate) override;

    /**
     * Set the voice preset to use
     */
    void setPreset(int presetIndex);

    /**
     * Set macro values (0.0-1.0)
     */
    void setMacro1(float value) { macro1Value = value; }
    void setMacro2(float value) { macro2Value = value; }
    void setMacro3(float value) { macro3Value = value; }
    void setMacro4(float value) { macro4Value = value; }
    void setMacro5(float value) { macro5Value = value; }
    void setMacro6(float value) { macro6Value = value; }
    void setMacro7(float value) { macro7Value = value; }

    /**
     * Set pitch bend value (-1.0 to +1.0, maps to ±2 semitones)
     */
    void setPitchBend(float semitones) { pitchBendSemitones = semitones; }

    /**
     * Get current RMS amplitude for voice stealing
     */
    float getCurrentRMS() const { return currentRMS; }

  private:
    // Current preset
    int currentPresetIndex = 0;

    // Note state
    float noteFrequency = 440.0f;
    float noteVelocity = 0.0f;
    bool isNoteOn = false;

    // Macro values
    float macro1Value = 0.5f;
    float macro2Value = 0.5f;
    float macro3Value = 0.5f;
    float macro4Value = 0.5f;
    float macro5Value = 0.5f;
    float macro6Value = 0.5f;
    float macro7Value = 0.5f;

    // Pitch bend
    float pitchBendSemitones = 0.0f;

    // Oscillator phases (up to 4 oscillators, up to 8 voices each)
    float oscPhases[4][8] = {{0.0f}};

    // Envelopes
    juce::ADSR ampEnvelope;
    juce::ADSR filterEnvelope;

    // Filter
    juce::dsp::StateVariableTPTFilter<float> filter;

    // LFO
    float lfoPhase = 0.0f;
    float lfoValue = 0.0f;

    // RMS calculation
    float currentRMS = 0.0f;
    static constexpr int rmsWindowSize = 64;
    float rmsBuffer[rmsWindowSize] = {0.0f};
    int rmsBufferIndex = 0;

    // Random generator for noise
    juce::Random random;

    // Helper methods
    float generateOscillatorSample(const OscillatorConfig &config, int oscIndex,
                                   float frequency);
    float generateWaveform(Waveform waveform, float phase);
    float applyMacroModulation(float baseValue, MacroParamType paramType,
                               const VoicePreset &preset);
    void updateLFO();
    void updateRMS(float sample);
};

} // namespace internal_plugins
