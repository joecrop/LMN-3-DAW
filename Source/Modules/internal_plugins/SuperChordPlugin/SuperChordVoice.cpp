#include "SuperChordVoice.h"
#include "SuperChordSound.h"
#include <cmath>

namespace internal_plugins {

SuperChordVoice::SuperChordVoice() {
    // Initialize filter
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filter.setCutoffFrequency(2000.0f);
    filter.setResonance(1.0f);
}

bool SuperChordVoice::canPlaySound(juce::SynthesiserSound *sound) {
    return dynamic_cast<SuperChordSound *>(sound) != nullptr;
}

void SuperChordVoice::setCurrentPlaybackSampleRate(double newRate) {
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);

    if (newRate > 0) {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = newRate;
        spec.maximumBlockSize = 512;
        spec.numChannels = 1;
        filter.prepare(spec);
    }
}

void SuperChordVoice::setPreset(int presetIndex) {
    currentPresetIndex = juce::jlimit(0, 15, presetIndex);
}

void SuperChordVoice::startNote(int midiNoteNumber, float velocity,
                                juce::SynthesiserSound * /*sound*/,
                                int currentPitchWheelPosition) {
    // Calculate note frequency
    noteFrequency =
        juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    noteVelocity = velocity;
    isNoteOn = true;

    // Get preset
    const VoicePreset &preset = VoicePresets::getPreset(currentPresetIndex);

    // Configure amplitude envelope
    juce::ADSR::Parameters ampParams;
    ampParams.attack = preset.ampEnvelope.attack;
    ampParams.decay = preset.ampEnvelope.decay;
    ampParams.sustain = preset.ampEnvelope.sustain;
    ampParams.release = preset.ampEnvelope.release;
    ampEnvelope.setSampleRate(getSampleRate());
    ampEnvelope.setParameters(ampParams);
    ampEnvelope.noteOn();

    // Configure filter envelope
    juce::ADSR::Parameters filterParams;
    filterParams.attack = preset.filterEnvelope.attack;
    filterParams.decay = preset.filterEnvelope.decay;
    filterParams.sustain = preset.filterEnvelope.sustain;
    filterParams.release = preset.filterEnvelope.release;
    filterEnvelope.setSampleRate(getSampleRate());
    filterEnvelope.setParameters(filterParams);
    filterEnvelope.noteOn();

    // Reset oscillator phases
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            oscPhases[i][j] = preset.oscillators[i].phase +
                              (j * 0.125f); // Spread phases for stereo width
        }
    }

    // Reset LFO
    lfoPhase = 0.0f;

    // Handle pitch wheel
    pitchWheelMoved(currentPitchWheelPosition);

    // Configure filter type
    switch (preset.filter.type) {
    case FilterType::LowPass:
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        break;
    case FilterType::HighPass:
        filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        break;
    case FilterType::BandPass:
        filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        break;
    }

    filter.setCutoffFrequency(preset.filter.cutoff);
    filter.setResonance(preset.filter.resonance);
}

void SuperChordVoice::stopNote(float /*velocity*/, bool allowTailOff) {
    if (allowTailOff) {
        ampEnvelope.noteOff();
        filterEnvelope.noteOff();
    } else {
        clearCurrentNote();
        ampEnvelope.reset();
        filterEnvelope.reset();
        isNoteOn = false;
    }
}

void SuperChordVoice::pitchWheelMoved(int newPitchWheelValue) {
    // Convert 14-bit pitch wheel value to semitones (-2 to +2)
    float normalized = (newPitchWheelValue - 8192) / 8192.0f;
    pitchBendSemitones = normalized * 2.0f;
}

void SuperChordVoice::controllerMoved(int /*controllerNumber*/,
                                      int /*newControllerValue*/) {
    // Handle MIDI CC if needed
}

float SuperChordVoice::generateWaveform(Waveform waveform, float phase) {
    switch (waveform) {
    case Waveform::Sine:
        return std::sin(phase * juce::MathConstants<float>::twoPi);

    case Waveform::Saw:
        return 2.0f * (phase - std::floor(phase + 0.5f));

    case Waveform::Square:
        return phase < 0.5f ? 1.0f : -1.0f;

    case Waveform::Triangle:
        return 4.0f * std::abs(phase - 0.5f) - 1.0f;

    case Waveform::Noise:
        return random.nextFloat() * 2.0f - 1.0f;

    default:
        return 0.0f;
    }
}

float SuperChordVoice::generateOscillatorSample(const OscillatorConfig &config,
                                                int oscIndex, float frequency) {
    float sample = 0.0f;
    double sampleRate = getSampleRate();

    if (sampleRate <= 0)
        return 0.0f;

    // Calculate frequency with multiplier and pitch bend
    float pitchMultiplier =
        std::pow(2.0f, pitchBendSemitones / 12.0f);
    float oscFrequency = frequency * config.freqMultiplier * pitchMultiplier;

    // Generate samples from all voices
    for (int voice = 0; voice < config.numVoices; voice++) {
        // Apply detune (spread across voices)
        float detuneAmount =
            config.detune * (voice - (config.numVoices - 1) * 0.5f) * 0.5f;
        float voiceFrequency = oscFrequency * std::pow(2.0f, detuneAmount);

        // Get phase for this oscillator voice
        float &phase = oscPhases[oscIndex][voice];

        // Generate waveform
        sample += generateWaveform(config.waveform, phase);

        // Advance phase
        phase += static_cast<float>(voiceFrequency / sampleRate);
        while (phase >= 1.0f)
            phase -= 1.0f;
        while (phase < 0.0f)
            phase += 1.0f;
    }

    // Normalize by number of voices
    if (config.numVoices > 0) {
        sample /= static_cast<float>(config.numVoices);
    }

    return sample * config.level;
}

float SuperChordVoice::applyMacroModulation(float baseValue,
                                            MacroParamType paramType,
                                            const VoicePreset &preset) {
    float macroValues[3] = {macro1Value, macro2Value, macro3Value};
    float result = baseValue;

    // Check each macro for targets matching this parameter
    for (int m = 0; m < 3; m++) {
        const MacroDefinition &macro = preset.macros[m];
        float macroVal = macroValues[m];

        for (const MacroTarget &target : macro.targets) {
            if (target.paramType == paramType) {
                // Apply curve
                float curvedValue = macroVal;
                switch (target.curve) {
                case MacroCurve::Linear:
                    curvedValue = macroVal;
                    break;
                case MacroCurve::Exponential:
                    curvedValue = macroVal * macroVal;
                    break;
                case MacroCurve::Logarithmic:
                    curvedValue = std::sqrt(macroVal);
                    break;
                }

                // Interpolate between min and max
                result = target.minValue +
                         curvedValue * (target.maxValue - target.minValue);
            }
        }
    }

    return result;
}

void SuperChordVoice::updateLFO() {
    const VoicePreset &preset = VoicePresets::getPreset(currentPresetIndex);
    double sampleRate = getSampleRate();

    if (sampleRate <= 0 || preset.lfo.rate <= 0)
        return;

    // Generate LFO waveform
    lfoValue = generateWaveform(preset.lfo.waveform, lfoPhase);

    // Advance LFO phase
    lfoPhase += static_cast<float>(preset.lfo.rate / sampleRate);
    while (lfoPhase >= 1.0f)
        lfoPhase -= 1.0f;
}

void SuperChordVoice::updateRMS(float sample) {
    // Update circular buffer
    rmsBuffer[rmsBufferIndex] = sample * sample;
    rmsBufferIndex = (rmsBufferIndex + 1) % rmsWindowSize;

    // Calculate RMS
    float sum = 0.0f;
    for (int i = 0; i < rmsWindowSize; i++) {
        sum += rmsBuffer[i];
    }
    currentRMS = std::sqrt(sum / rmsWindowSize);
}

void SuperChordVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                                      int startSample, int numSamples) {
    if (!isNoteOn && !ampEnvelope.isActive())
        return;

    const VoicePreset &preset = VoicePresets::getPreset(currentPresetIndex);

    // Apply macro modulation to LFO parameters (constant for block)
    float lfoRate = applyMacroModulation(preset.lfo.rate,
                                         MacroParamType::LFORate, preset);
    float lfoDepth = applyMacroModulation(preset.lfo.depth,
                                          MacroParamType::LFODepth, preset);

    // Apply macro modulation to filter resonance (constant for block)
    float filterResonance = applyMacroModulation(preset.filter.resonance,
                                                  MacroParamType::FilterResonance, preset);
    filter.setResonance(juce::jmax(0.5f, filterResonance));

    for (int sample = 0; sample < numSamples; sample++) {
        // Update LFO with macro-modulated rate
        double sampleRate = getSampleRate();
        if (sampleRate > 0 && lfoRate > 0) {
            lfoValue = generateWaveform(preset.lfo.waveform, lfoPhase);
            lfoPhase += static_cast<float>(lfoRate / sampleRate);
            while (lfoPhase >= 1.0f)
                lfoPhase -= 1.0f;
        }

        // Generate oscillator mix with macro-modulated parameters
        float oscMix = 0.0f;
        float oscDetuneModulated = applyMacroModulation(0.0f, MacroParamType::OscDetune, preset);
        float oscLevelModulated = applyMacroModulation(1.0f, MacroParamType::OscLevel, preset);

        for (int osc = 0; osc < preset.numOscillators; osc++) {
            OscillatorConfig modConfig = preset.oscillators[osc];
            // Apply macro detune (additive)
            modConfig.detune = modConfig.detune + oscDetuneModulated;
            // Apply macro level (multiplicative)
            modConfig.level = modConfig.level * oscLevelModulated;
            oscMix += generateOscillatorSample(modConfig, osc, noteFrequency);
        }

        // Apply velocity
        oscMix *= noteVelocity;

        // Get envelope values
        float ampEnvValue = ampEnvelope.getNextSample();
        float filterEnvValue = filterEnvelope.getNextSample();

        // Calculate filter cutoff with modulation
        float baseCutoff =
            applyMacroModulation(preset.filter.cutoff,
                                 MacroParamType::FilterCutoff, preset);

        // Apply filter envelope
        float filterEnvAmount = preset.filter.envAmount;
        float envModulation = filterEnvValue * filterEnvAmount * 5000.0f;

        // Apply LFO modulation to filter with macro-modulated depth
        float lfoFilterMod = 0.0f;
        if (preset.lfo.targetFilterCutoff) {
            lfoFilterMod = lfoValue * lfoDepth * 4000.0f;
        }

        float finalCutoff = juce::jlimit(
            20.0f, 20000.0f, baseCutoff + envModulation + lfoFilterMod);

        filter.setCutoffFrequency(finalCutoff);

        // Apply filter
        float filteredSample = filter.processSample(0, oscMix);

        // Apply amplitude envelope
        float finalSample = filteredSample * ampEnvValue;

        // Apply LFO to amplitude with macro-modulated depth
        if (preset.lfo.targetOscLevel) {
            float lfoAmpMod = 1.0f + (lfoValue * lfoDepth * 0.5f);
            finalSample *= lfoAmpMod;
        }

        // Update RMS
        updateRMS(finalSample);

        // Write to output (both channels)
        for (int channel = 0; channel < outputBuffer.getNumChannels();
             channel++) {
            outputBuffer.addSample(channel, startSample + sample, finalSample);
        }
    }

    // Check if note has finished
    if (!ampEnvelope.isActive()) {
        clearCurrentNote();
        isNoteOn = false;
    }
}

} // namespace internal_plugins
