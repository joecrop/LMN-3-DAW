#pragma once

#include "VoicePresets.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace internal_plugins {

class SuperChordPlugin; // Forward declaration

/**
 * SuperChordVoice - A single synthesiser voice with multi-oscillator
 * architecture.
 *
 * Features layered oscillators (sine, saw, triangle, square) with
 * parameters controlled by the 4 macro controls:
 * - Warmth: Filter cutoff, oscillator mix
 * - Space: Detune, stereo spread
 * - Attack: Envelope attack time
 * - Bloom: Envelope release time
 */
class SuperChordVoice : public juce::SynthesiserVoice {
  public:
    SuperChordVoice(SuperChordPlugin &plugin);

    //==========================================================================
    // SynthesiserVoice overrides
    bool canPlaySound(juce::SynthesiserSound *sound) override;

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *,
                   int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                         int startSample, int numSamples) override;

    //==========================================================================
    // Parameter updates from plugin
    void updateParameters(float warmth, float space, float attack, float bloom,
                          const VoicePreset &preset);

  private:
    // Calculate frequency from MIDI note
    float midiNoteToFrequency(int midiNote) const;

    // Generate oscillator sample
    float generateOscillator(float phase, float pulseWidth, int waveform) const;

    // Apply filter to sample
    float applyFilter(float sample);

    SuperChordPlugin &pluginRef;

    // Oscillator state
    float phase1 = 0.0f;
    float phase2 = 0.0f;
    float phase3 = 0.0f;
    float phase4 = 0.0f;

    float frequency = 440.0f;
    float velocity = 0.0f;
    int currentNote = 60;

    // Detuned oscillator phases for unison
    std::array<float, 4> detunePhases = {0.0f, 0.0f, 0.0f, 0.0f};
    std::array<float, 4> detuneOffsets = {-0.02f, -0.01f, 0.01f, 0.02f};

    // ADSR envelope
    juce::ADSR envelope;
    juce::ADSR::Parameters envelopeParams;

    // Filter
    juce::dsp::StateVariableTPTFilter<float> filter;
    float filterCutoff = 2000.0f;
    float filterResonance = 0.5f;

    // Current macro values
    float currentWarmth = 0.5f;
    float currentSpace = 0.3f;
    float currentAttack = 0.4f;
    float currentBloom = 0.5f;

    // Oscillator mix (from preset + warmth)
    float sineLevel = 0.5f;
    float sawLevel = 0.3f;
    float triangleLevel = 0.2f;
    float squareLevel = 0.0f;

    // Stereo spread
    float panPosition = 0.5f;

    // Pitch wheel
    float pitchWheelSemitones = 0.0f;

    // Tail off state
    bool isTailingOff = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChordVoice)
};

} // namespace internal_plugins
