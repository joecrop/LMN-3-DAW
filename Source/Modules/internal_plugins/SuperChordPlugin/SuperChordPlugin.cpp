#include "SuperChordPlugin.h"
#include "ChordEngine.h"
#include "SuperChordSound.h"
#include "SuperChordVoice.h"
#include "VoicePresets.h"

namespace internal_plugins {

const char *SuperChordPlugin::xmlTypeName = "superChord";

SuperChordPlugin::SuperChordPlugin(tracktion::PluginCreationInfo info)
    : tracktion::Plugin(info) {
    auto um = getUndoManager();

    // Initialize cached values with defaults
    voicePresetValue.referTo(state, juce::Identifier("voicePreset"), um, 1.0f);
    macro1Value.referTo(state, juce::Identifier("macro1"), um, 0.5f);
    macro2Value.referTo(state, juce::Identifier("macro2"), um, 0.5f);
    macro3Value.referTo(state, juce::Identifier("macro3"), um, 0.5f);
    voicingValue.referTo(state, juce::Identifier("voicing"), um, 0.0f);
    octaveValue.referTo(state, juce::Identifier("octave"), um, 0.0f);
    keyValue.referTo(state, juce::Identifier("key"), um, 0.0f);
    extensionValue.referTo(state, juce::Identifier("extension"), um, 0.0f);
    arpModeValue.referTo(state, juce::Identifier("arpMode"), um, 0.0f);
    arpDirectionValue.referTo(state, juce::Identifier("arpDirection"), um, 0.0f);
    arpStepsValue.referTo(state, juce::Identifier("arpSteps"), um, 8.0f);
    arpRateValue.referTo(state, juce::Identifier("arpRate"), um, 2.0f);

    // Helper lambda to create automatable parameters
    auto createParam = [this](const juce::String& paramID, const juce::String& name,
                              juce::NormalisableRange<float> range) -> tracktion::AutomatableParameter::Ptr {
        auto p = new tracktion::AutomatableParameter(paramID, name, *this, range);
        addAutomatableParameter(p);
        return p;
    };

    // Create automatable parameters
    voicePresetParam = createParam("voicePreset", TRANS("Voice Preset"), {1.0f, 64.0f});
    voicePresetParam->attachToCurrentValue(voicePresetValue);

    macro1Param = createParam("macro1", TRANS("Macro 1"), {0.0f, 1.0f});
    macro1Param->attachToCurrentValue(macro1Value);

    macro2Param = createParam("macro2", TRANS("Macro 2"), {0.0f, 1.0f});
    macro2Param->attachToCurrentValue(macro2Value);

    macro3Param = createParam("macro3", TRANS("Macro 3"), {0.0f, 1.0f});
    macro3Param->attachToCurrentValue(macro3Value);

    voicingParam = createParam("voicing", TRANS("Voicing"), {0.0f, 2.0f});
    voicingParam->attachToCurrentValue(voicingValue);

    octaveParam = createParam("octave", TRANS("Octave"), {-1.0f, 1.0f});
    octaveParam->attachToCurrentValue(octaveValue);

    keyParam = createParam("key", TRANS("Key"), {0.0f, 11.0f});
    keyParam->attachToCurrentValue(keyValue);

    extensionParam = createParam("extension", TRANS("Extension"), {0.0f, 2.0f});
    extensionParam->attachToCurrentValue(extensionValue);

    arpModeParam = createParam("arpMode", TRANS("Arp Mode"), {0.0f, 3.0f});
    arpModeParam->attachToCurrentValue(arpModeValue);

    arpDirectionParam = createParam("arpDirection", TRANS("Arp Direction"), {0.0f, 3.0f});
    arpDirectionParam->attachToCurrentValue(arpDirectionValue);

    arpStepsParam = createParam("arpSteps", TRANS("Arp Steps"), {2.0f, 16.0f});
    arpStepsParam->attachToCurrentValue(arpStepsValue);

    arpRateParam = createParam("arpRate", TRANS("Arp Rate"), {0.0f, 6.0f});
    arpRateParam->attachToCurrentValue(arpRateValue);

    // Initialize chord engine
    chordEngine = std::make_unique<ChordEngine>();

    // Initialize synthesiser with 32 voices
    for (int i = 0; i < 32; i++) {
        synthesiser.addVoice(new SuperChordVoice());
    }
    synthesiser.addSound(new SuperChordSound());
}

SuperChordPlugin::~SuperChordPlugin() {
    notifyListenersOfDeletion();

    // Detach all parameters
    voicePresetParam->detachFromCurrentValue();
    macro1Param->detachFromCurrentValue();
    macro2Param->detachFromCurrentValue();
    macro3Param->detachFromCurrentValue();
    voicingParam->detachFromCurrentValue();
    octaveParam->detachFromCurrentValue();
    keyParam->detachFromCurrentValue();
    extensionParam->detachFromCurrentValue();
    arpModeParam->detachFromCurrentValue();
    arpDirectionParam->detachFromCurrentValue();
    arpStepsParam->detachFromCurrentValue();
    arpRateParam->detachFromCurrentValue();
}

void SuperChordPlugin::initialise(
    const tracktion::PluginInitialisationInfo &info) {
    sampleRate = info.sampleRate;
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
}

void SuperChordPlugin::deinitialise() {}

void SuperChordPlugin::reset() {
    synthesiser.allNotesOff(0, false);
    activeChordNotes.clear();
    latchedNotes.clear();
    currentArpStep = 0;
    arpSampleCounter = 0.0;
    currentChordDegree = -1;
    currentChordName = "";
}

// Parameter accessors
int SuperChordPlugin::getCurrentVoicePresetIndex() const {
    return static_cast<int>(voicePresetValue.get()) - 1;
}

float SuperChordPlugin::getMacro1Value() const { return macro1Value.get(); }
float SuperChordPlugin::getMacro2Value() const { return macro2Value.get(); }
float SuperChordPlugin::getMacro3Value() const { return macro3Value.get(); }

int SuperChordPlugin::getVoicingValue() const {
    return static_cast<int>(voicingValue.get());
}

int SuperChordPlugin::getOctaveValue() const {
    return static_cast<int>(octaveValue.get());
}

int SuperChordPlugin::getKeyValue() const {
    return static_cast<int>(keyValue.get());
}

int SuperChordPlugin::getExtensionValue() const {
    return static_cast<int>(extensionValue.get());
}

int SuperChordPlugin::getArpModeValue() const {
    return static_cast<int>(arpModeValue.get());
}

int SuperChordPlugin::getArpDirectionValue() const {
    return static_cast<int>(arpDirectionValue.get());
}

int SuperChordPlugin::getArpStepsValue() const {
    return static_cast<int>(arpStepsValue.get());
}

int SuperChordPlugin::getArpRateValue() const {
    return static_cast<int>(arpRateValue.get());
}

juce::String SuperChordPlugin::getCurrentPresetName() const {
    return VoicePresets::getPresetName(getCurrentVoicePresetIndex());
}

double SuperChordPlugin::calculateArpInterval(double bpm) const {
    // Rate values: 0=1/32, 1=1/16, 2=1/8, 3=1/4, 4=1/2, 5=1/1, 6=2/1
    static const double rateMultipliers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0};
    int rateIndex = getArpRateValue();
    rateIndex = juce::jlimit(0, 6, rateIndex);

    // Calculate PPQ interval (4 PPQ = 1 beat = 1/4 note)
    double beatsPerNote = rateMultipliers[rateIndex];
    return beatsPerNote;
}

void SuperChordPlugin::processNoteOn(int midiNote, float velocity,
                                     double currentPPQ, double bpm,
                                     int samplePosition) {
    // Map MIDI notes to chord degrees I-VII across 3 octaves around middle C
    // C3 (48) to B5 (83) - notes outside this range are ignored
    // Lower octave (C3-B3): chord transposed down 1 octave
    // Middle octave (C4-B4): chord at normal pitch
    // Upper octave (C5-B5): chord transposed up 1 octave
    
    // C=I, D=II, E=III, F=IV, G=V, A=VI, B=VII
    // Black keys are mapped to the lower white key's degree
    static const int noteToChordDegree[] = {
        0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6  // C, C#, D, D#, E, F, F#, G, G#, A, A#, B
    };
    
    // Only respond to notes in the 3-octave range (C3 to B5)
    if (midiNote < 48 || midiNote > 83)
        return;
    
    int noteInOctave = midiNote % 12;
    int degree = noteToChordDegree[noteInOctave];
    currentChordDegree = degree;
    
    // Calculate octave offset based on which octave the note is in
    // C3-B3 (48-59) = -1, C4-B4 (60-71) = 0, C5-B5 (72-83) = +1
    int keyboardOctave = (midiNote - 48) / 12 - 1;  // -1, 0, or +1

    // Get current parameters
    int key = getKeyValue();
    auto voicing = static_cast<ChordVoicing>(getVoicingValue());
    auto extension = static_cast<ChordExtension>(getExtensionValue());
    int octaveParam = getOctaveValue();
    // Combine keyboard octave with the octave parameter
    int totalOctaveOffset = octaveParam + keyboardOctave;
    int presetIndex = getCurrentVoicePresetIndex();

    // Generate chord
    activeChordNotes =
        chordEngine->generateChord(degree, key, voicing, extension, totalOctaveOffset);
    currentChordName = chordEngine->getChordName(degree, key, extension);

    // Update all voices with current preset and macro values
    for (int i = 0; i < synthesiser.getNumVoices(); i++) {
        if (auto *voice =
                dynamic_cast<SuperChordVoice *>(synthesiser.getVoice(i))) {
            voice->setPreset(presetIndex);
            voice->setMacro1(getMacro1Value());
            voice->setMacro2(getMacro2Value());
            voice->setMacro3(getMacro3Value());
        }
    }

    int arpMode = getArpModeValue();

    switch (arpMode) {
    case 0: // Off - play all notes simultaneously
        for (int note : activeChordNotes) {
            synthesiser.noteOn(1, note, velocity);
        }
        break;

    case 1: // Arpeggiate
        currentArpStep = 0;
        arpSampleCounter = 0.0;
        arpDirectionUp = true;
        // First note triggers immediately
        if (!activeChordNotes.isEmpty()) {
            synthesiser.noteOn(1, activeChordNotes[0], velocity);
        }
        break;

    case 2: // Strum - trigger with slight delays
        {
            // Note: In a real implementation, we'd need sample-accurate timing
            // For simplicity, trigger all with synthesiser's internal handling
            for (int i = 0; i < activeChordNotes.size(); i++) {
                synthesiser.noteOn(1, activeChordNotes[i], velocity);
            }
        }
        break;

    case 3: // Latch
        // Clear previous latched notes
        for (int note : latchedNotes) {
            synthesiser.noteOff(1, note, 0.0f, false);
        }
        latchedNotes.clear();

        // Play new chord and latch
        for (int note : activeChordNotes) {
            synthesiser.noteOn(1, note, velocity);
            latchedNotes.add(note);
        }
        break;
    }
}

void SuperChordPlugin::processNoteOff(int midiNote, float velocity) {
    // Only respond to notes in the 3-octave range (C3 to B5)
    if (midiNote < 48 || midiNote > 83)
        return;
        
    int arpMode = getArpModeValue();

    // Latch mode ignores note-off
    if (arpMode == 3)
        return;

    // Turn off all active chord notes
    for (int note : activeChordNotes) {
        synthesiser.noteOff(1, note, velocity, true);
    }

    activeChordNotes.clear();
    currentArpStep = 0;
    currentChordDegree = -1;
}

void SuperChordPlugin::processPitchWheel(int rawValue) {
    // Convert 14-bit pitch wheel to normalized value
    float normalized = (rawValue - 8192) / 8192.0f;
    pitchWheelValue = juce::jlimit(-1.0f, 1.0f, normalized);

    // Apply to all active voices
    for (int i = 0; i < synthesiser.getNumVoices(); i++) {
        if (auto *voice =
                dynamic_cast<SuperChordVoice *>(synthesiser.getVoice(i))) {
            voice->setPitchBend(pitchWheelValue * 2.0f); // ±2 semitones
        }
    }
}

void SuperChordPlugin::triggerArpStep(double /*currentPPQ*/, double /*bpm*/,
                                      int /*samplePosition*/) {
    if (activeChordNotes.isEmpty())
        return;

    int arpDirection = getArpDirectionValue();
    int numChordNotes = activeChordNotes.size();
    int arpSteps = getArpStepsValue();
    
    // The arpeggiator cycles through arpSteps positions, wrapping around the chord notes
    // This allows longer patterns that repeat through the chord
    int numSteps = juce::jmax(1, arpSteps);

    // Turn off previous note (simple implementation)
    int prevNoteIndex = currentArpStep % numChordNotes;

    // Advance step based on direction
    switch (arpDirection) {
    case 0: // Up
        currentArpStep = (currentArpStep + 1) % numSteps;
        break;

    case 1: // Down
        currentArpStep = (currentArpStep - 1 + numSteps) % numSteps;
        break;

    case 2: // Up-Down
        if (arpDirectionUp) {
            currentArpStep++;
            if (currentArpStep >= numSteps - 1) {
                currentArpStep = numSteps - 1;
                arpDirectionUp = false;
            }
        } else {
            currentArpStep--;
            if (currentArpStep <= 0) {
                currentArpStep = 0;
                arpDirectionUp = true;
            }
        }
        break;

    case 3: // Random
        currentArpStep = juce::Random::getSystemRandom().nextInt(numSteps);
        break;
    }

    // Map step to note index (wrap around chord notes)
    int currentNoteIndex = currentArpStep % numChordNotes;

    // Turn off previous note
    if (prevNoteIndex >= 0 && prevNoteIndex < numChordNotes) {
        synthesiser.noteOff(1, activeChordNotes[prevNoteIndex], 0.0f, true);
    }

    // Trigger current note
    if (currentNoteIndex >= 0 && currentNoteIndex < numChordNotes) {
        synthesiser.noteOn(1, activeChordNotes[currentNoteIndex], 0.8f);
    }
}

void SuperChordPlugin::applyToBuffer(const tracktion::PluginRenderContext &fc) {
    if (!isEnabled())
        return;

    // Get tempo info from the edit at the current edit time
    auto& tempo = edit.tempoSequence.getTempoAt(fc.editTime);
    double bpm = tempo.getBpm();

    // Process MIDI input
    if (fc.bufferForMidiMessages != nullptr) {
        for (auto &msg : *fc.bufferForMidiMessages) {
            if (msg.isNoteOn()) {
                processNoteOn(msg.getNoteNumber(), msg.getFloatVelocity(),
                              0.0, bpm, 0);
            } else if (msg.isNoteOff()) {
                processNoteOff(msg.getNoteNumber(), msg.getFloatVelocity());
            } else if (msg.isPitchWheel()) {
                processPitchWheel(msg.getPitchWheelValue());
            }
        }
    }

    // Handle arpeggiator timing (Mode 1 = Arpeggiate)
    // Works both when playing and stopped using sample-based timing
    if (getArpModeValue() == 1 && !activeChordNotes.isEmpty()) {
        // Calculate samples per arp step based on tempo and rate
        double arpInterval = calculateArpInterval(bpm);
        double beatsPerSecond = bpm / 60.0;
        double secondsPerArpStep = arpInterval / beatsPerSecond;
        double samplesPerArpStep = secondsPerArpStep * sampleRate;
        
        // Process arpeggiator for this buffer
        arpSampleCounter += fc.bufferNumSamples;
        
        while (arpSampleCounter >= samplesPerArpStep) {
            arpSampleCounter -= samplesPerArpStep;
            triggerArpStep(0.0, bpm, 0);
        }
    }

    // Update voice parameters
    for (int i = 0; i < synthesiser.getNumVoices(); i++) {
        if (auto *voice =
                dynamic_cast<SuperChordVoice *>(synthesiser.getVoice(i))) {
            voice->setPreset(getCurrentVoicePresetIndex());
            voice->setMacro1(getMacro1Value());
            voice->setMacro2(getMacro2Value());
            voice->setMacro3(getMacro3Value());
        }
    }

    // Render audio
    juce::MidiBuffer emptyMidiBuffer;
    synthesiser.renderNextBlock(*fc.destBuffer, emptyMidiBuffer, 0,
                                fc.bufferNumSamples);
}

void SuperChordPlugin::restorePluginStateFromValueTree(
    const juce::ValueTree &v) {
    juce::CachedValue<float> *cvsFloat[] = {
        &voicePresetValue, &macro1Value,       &macro2Value,
        &macro3Value,      &voicingValue,      &octaveValue,
        &keyValue,         &extensionValue,    &arpModeValue,
        &arpDirectionValue, &arpStepsValue,    &arpRateValue,
        nullptr};
    tracktion::copyPropertiesToNullTerminatedCachedValues(v, cvsFloat);

    for (auto p : getAutomatableParameters())
        p->updateFromAttachedValue();
}

} // namespace internal_plugins
