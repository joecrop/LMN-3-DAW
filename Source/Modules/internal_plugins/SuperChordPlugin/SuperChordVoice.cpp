#include "SuperChordVoice.h"
#include "SuperChordPlugin.h"
#include "SuperChordSound.h"

namespace internal_plugins {

SuperChordVoice::SuperChordVoice(SuperChordPlugin &plugin) : pluginRef(plugin) {
    // Initialize envelope with default parameters
    envelopeParams.attack = 0.1f;
    envelopeParams.decay = 0.1f;
    envelopeParams.sustain = 0.8f;
    envelopeParams.release = 0.5f;
    envelope.setParameters(envelopeParams);
}

//==============================================================================
bool SuperChordVoice::canPlaySound(juce::SynthesiserSound *sound) {
    return dynamic_cast<SuperChordSound *>(sound) != nullptr;
}

//==============================================================================
void SuperChordVoice::startNote(int midiNoteNumber, float vel,
                                juce::SynthesiserSound *,
                                int currentPitchWheelPosition) {
    currentNote = midiNoteNumber;
    velocity = vel;
    frequency = midiNoteToFrequency(midiNoteNumber);

    // Reset oscillator phases with slight random offset for organic feel
    phase1 = juce::Random::getSystemRandom().nextFloat() * 0.1f;
    phase2 = juce::Random::getSystemRandom().nextFloat() * 0.1f;
    phase3 = juce::Random::getSystemRandom().nextFloat() * 0.1f;
    phase4 = juce::Random::getSystemRandom().nextFloat() * 0.1f;

    // Reset detune phases
    for (auto &p : detunePhases)
        p = juce::Random::getSystemRandom().nextFloat() * 0.1f;

    // Calculate pan based on note position (higher notes slightly right)
    float noteNormalized = (midiNoteNumber - 36) / 60.0f; // C2 to C7 range
    panPosition = 0.3f + noteNormalized * 0.4f; // 0.3 to 0.7 range

    // Apply space parameter to spread
    panPosition += (currentSpace - 0.5f) * 0.3f;
    panPosition = juce::jlimit(0.0f, 1.0f, panPosition);

    // Process pitch wheel
    pitchWheelMoved(currentPitchWheelPosition);

    // Reset filter state
    lastFilteredSample = 0.0f;

    // Start envelope
    double sr = getSampleRate();
    if (sr <= 0.0) sr = 44100.0;
    envelope.setSampleRate(sr);
    envelope.noteOn();
    isTailingOff = false;
}

void SuperChordVoice::stopNote(float /*velocity*/, bool allowTailOff) {
    if (allowTailOff) {
        envelope.noteOff();
        isTailingOff = true;
    } else {
        envelope.reset();
        clearCurrentNote();
    }
}

//==============================================================================
void SuperChordVoice::pitchWheelMoved(int newPitchWheelValue) {
    // Convert 0-16383 to -2 to +2 semitones
    float normalized = (newPitchWheelValue - 8192) / 8192.0f;
    pitchWheelSemitones = normalized * 2.0f;
}

void SuperChordVoice::controllerMoved(int /*controllerNumber*/,
                                      int /*newControllerValue*/) {
    // Not used - parameters come from plugin macros
}

//==============================================================================
void SuperChordVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                                      int startSample, int numSamples) {
    if (!isVoiceActive())
        return;

    // Calculate actual frequency with pitch wheel
    float pitchMultiplier =
        std::pow(2.0f, pitchWheelSemitones / 12.0f);
    float actualFreq = frequency * pitchMultiplier;

    // Calculate phase increment
    float sampleRate = static_cast<float>(getSampleRate());
    if (sampleRate <= 0.0f) sampleRate = 44100.0f;
    float phaseIncrement = actualFreq / sampleRate;

    // Detune amounts based on space parameter
    float detuneAmount = currentSpace * 0.03f; // 0-3% detune

    for (int sample = 0; sample < numSamples; ++sample) {
        // Get envelope value
        float envValue = envelope.getNextSample();

        if (isTailingOff && envValue < 0.001f) {
            clearCurrentNote();
            break;
        }

        // Generate main oscillator mix
        float oscSample = 0.0f;

        // Sine oscillator
        if (sineLevel > 0.01f) {
            oscSample += sineLevel * std::sin(phase1 * juce::MathConstants<float>::twoPi);
        }

        // Saw oscillator
        if (sawLevel > 0.01f) {
            oscSample += sawLevel * (2.0f * phase2 - 1.0f);
        }

        // Triangle oscillator
        if (triangleLevel > 0.01f) {
            float triValue = 2.0f * std::abs(2.0f * phase3 - 1.0f) - 1.0f;
            oscSample += triangleLevel * triValue;
        }

        // Square oscillator
        if (squareLevel > 0.01f) {
            oscSample += squareLevel * (phase4 < 0.5f ? 1.0f : -1.0f);
        }

        // Add detuned unison voices for richness (space parameter)
        if (currentSpace > 0.1f) {
            for (int i = 0; i < 4; ++i) {
                float unisonSample =
                    std::sin(detunePhases[i] * juce::MathConstants<float>::twoPi);
                oscSample += unisonSample * currentSpace * 0.15f;
            }
        }

        // Apply filter based on warmth
        float filteredSample = applyFilter(oscSample);

        // Apply envelope and velocity
        float finalSample = filteredSample * envValue * velocity * 0.3f;

        // Apply to output with stereo panning
        float leftGain = std::cos(panPosition * juce::MathConstants<float>::halfPi);
        float rightGain = std::sin(panPosition * juce::MathConstants<float>::halfPi);

        if (outputBuffer.getNumChannels() >= 2) {
            outputBuffer.addSample(0, startSample + sample, finalSample * leftGain);
            outputBuffer.addSample(1, startSample + sample, finalSample * rightGain);
        } else if (outputBuffer.getNumChannels() == 1) {
            outputBuffer.addSample(0, startSample + sample, finalSample);
        }

        // Update phases
        phase1 += phaseIncrement;
        phase2 += phaseIncrement;
        phase3 += phaseIncrement;
        phase4 += phaseIncrement;

        // Update detuned phases
        for (int i = 0; i < 4; ++i) {
            float detuneFreq = actualFreq * (1.0f + detuneOffsets[i] * detuneAmount);
            detunePhases[i] += detuneFreq / sampleRate;
            if (detunePhases[i] >= 1.0f)
                detunePhases[i] -= 1.0f;
        }

        // Wrap phases properly (handles cases where phase > 2.0)
        while (phase1 >= 1.0f) phase1 -= 1.0f;
        while (phase2 >= 1.0f) phase2 -= 1.0f;
        while (phase3 >= 1.0f) phase3 -= 1.0f;
        while (phase4 >= 1.0f) phase4 -= 1.0f;
    }
}

//==============================================================================
void SuperChordVoice::updateParameters(float warmth, float space, float attack,
                                       float bloom, const VoicePreset &preset) {
    currentWarmth = warmth;
    currentSpace = space;
    currentAttack = attack;
    currentBloom = bloom;

    // Update oscillator levels from preset + warmth adjustment
    sineLevel = preset.sineLevel * (1.0f + (1.0f - warmth) * 0.5f);
    sawLevel = preset.sawLevel * (1.0f + warmth * 0.5f);
    triangleLevel = preset.triangleLevel;
    squareLevel = preset.squareLevel * warmth * 0.5f;

    // Normalize levels
    float totalLevel = sineLevel + sawLevel + triangleLevel + squareLevel;
    if (totalLevel > 0.01f) {
        float scale = 1.0f / totalLevel;
        sineLevel *= scale;
        sawLevel *= scale;
        triangleLevel *= scale;
        squareLevel *= scale;
    }

    // Update filter based on warmth
    // Lower warmth = brighter (higher cutoff), higher warmth = darker
    filterCutoff = 500.0f + (1.0f - warmth) * 8000.0f;
    filterResonance = 0.3f + warmth * 0.4f;

    // Update envelope based on attack and bloom
    envelopeParams.attack = 0.001f + attack * 2.0f; // 1ms to 2s
    envelopeParams.decay = 0.05f + attack * 0.3f;
    envelopeParams.sustain = 0.5f + (1.0f - bloom) * 0.4f;
    envelopeParams.release = 0.05f + bloom * 5.0f; // 50ms to 5s
    envelope.setParameters(envelopeParams);

    // Update filter coefficients
    if (getSampleRate() > 0) {
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        filter.setCutoffFrequency(filterCutoff);
        filter.setResonance(filterResonance);
    }
}

//==============================================================================
float SuperChordVoice::midiNoteToFrequency(int midiNote) const {
    return 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
}

float SuperChordVoice::generateOscillator(float phase, float pulseWidth,
                                          int waveform) const {
    switch (waveform) {
    case 0: // Sine
        return std::sin(phase * juce::MathConstants<float>::twoPi);
    case 1: // Saw
        return 2.0f * phase - 1.0f;
    case 2: // Triangle
        return 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;
    case 3: // Square/Pulse
        return phase < pulseWidth ? 1.0f : -1.0f;
    default:
        return 0.0f;
    }
}

float SuperChordVoice::applyFilter(float sample) {
    // Simple one-pole lowpass - using per-voice state
    float sr = static_cast<float>(getSampleRate());
    if (sr <= 0.0f) sr = 44100.0f;
    float alpha = filterCutoff / (filterCutoff + sr);
    float filtered = lastFilteredSample + alpha * (sample - lastFilteredSample);
    lastFilteredSample = filtered;
    return filtered;
}

} // namespace internal_plugins
