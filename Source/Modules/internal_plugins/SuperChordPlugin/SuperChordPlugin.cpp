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
    macro4Value.referTo(state, juce::Identifier("macro4"), um, 0.5f);
    macro5Value.referTo(state, juce::Identifier("macro5"), um, 0.5f);
    macro6Value.referTo(state, juce::Identifier("macro6"), um, 0.5f);
    macro7Value.referTo(state, juce::Identifier("macro7"), um, 0.5f);
    voicingValue.referTo(state, juce::Identifier("voicing"), um, 0.0f);
    octaveValue.referTo(state, juce::Identifier("octave"), um, 0.0f);
    keyValue.referTo(state, juce::Identifier("key"), um, 0.0f);
    extensionValue.referTo(state, juce::Identifier("extension"), um, 0.0f);
    arpModeValue.referTo(state, juce::Identifier("arpMode"), um, 0.0f);
    arpDirectionValue.referTo(state, juce::Identifier("arpDirection"), um, 0.0f);
    arpStepsValue.referTo(state, juce::Identifier("arpSteps"), um, 8.0f);
    arpRateValue.referTo(state, juce::Identifier("arpRate"), um, 7.0f);  // Default to 1/8 note
    progressionValue.referTo(state, juce::Identifier("progression"), um, 0.0f);

    // Helper lambda to create automatable parameters
    auto createParam = [this](const juce::String& paramID, const juce::String& name,
                              juce::NormalisableRange<float> range) -> tracktion::AutomatableParameter::Ptr {
        auto p = new tracktion::AutomatableParameter(paramID, name, *this, range);
        addAutomatableParameter(p);
        return p;
    };

    // Create automatable parameters
    voicePresetParam = createParam("voicePreset", TRANS("Voice Preset"), {1.0f, 128.0f});
    voicePresetParam->attachToCurrentValue(voicePresetValue);

    macro1Param = createParam("macro1", TRANS("Macro 1"), {0.0f, 1.0f});
    macro1Param->attachToCurrentValue(macro1Value);

    macro2Param = createParam("macro2", TRANS("Macro 2"), {0.0f, 1.0f});
    macro2Param->attachToCurrentValue(macro2Value);

    macro3Param = createParam("macro3", TRANS("Macro 3"), {0.0f, 1.0f});
    macro3Param->attachToCurrentValue(macro3Value);

    macro4Param = createParam("macro4", TRANS("Macro 4"), {0.0f, 1.0f});
    macro4Param->attachToCurrentValue(macro4Value);

    macro5Param = createParam("macro5", TRANS("Macro 5"), {0.0f, 1.0f});
    macro5Param->attachToCurrentValue(macro5Value);

    macro6Param = createParam("macro6", TRANS("Macro 6"), {0.0f, 1.0f});
    macro6Param->attachToCurrentValue(macro6Value);

    macro7Param = createParam("macro7", TRANS("Macro 7"), {0.0f, 1.0f});
    macro7Param->attachToCurrentValue(macro7Value);

    voicingParam = createParam("voicing", TRANS("Voicing"), {0.0f, 2.0f});
    voicingParam->attachToCurrentValue(voicingValue);

    octaveParam = createParam("octave", TRANS("Octave"), {-3.0f, 3.0f});
    octaveParam->attachToCurrentValue(octaveValue);

    keyParam = createParam("key", TRANS("Key"), {0.0f, 11.0f});
    keyParam->attachToCurrentValue(keyValue);

    extensionParam = createParam("extension", TRANS("Extension"), {0.0f, 2.0f});
    extensionParam->attachToCurrentValue(extensionValue);

    arpModeParam = createParam("arpMode", TRANS("Arp Mode"), {0.0f, 3.0f});
    arpModeParam->attachToCurrentValue(arpModeValue);

    arpDirectionParam = createParam("arpDirection", TRANS("Arp Direction"), {0.0f, 19.0f});
    arpDirectionParam->attachToCurrentValue(arpDirectionValue);

    arpStepsParam = createParam("arpSteps", TRANS("Arp Steps"), {2.0f, 16.0f});
    arpStepsParam->attachToCurrentValue(arpStepsValue);

    arpRateParam = createParam("arpRate", TRANS("Arp Rate"), {0.0f, 13.0f});
    arpRateParam->attachToCurrentValue(arpRateValue);

    progressionParam = createParam("progression", TRANS("Progression"), {0.0f, 21.0f});
    progressionParam->attachToCurrentValue(progressionValue);

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
    macro4Param->detachFromCurrentValue();
    macro5Param->detachFromCurrentValue();
    macro6Param->detachFromCurrentValue();
    macro7Param->detachFromCurrentValue();
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
    midiNoteToChordNotes.clear();
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
float SuperChordPlugin::getMacro4Value() const { return macro4Value.get(); }
float SuperChordPlugin::getMacro5Value() const { return macro5Value.get(); }
float SuperChordPlugin::getMacro6Value() const { return macro6Value.get(); }
float SuperChordPlugin::getMacro7Value() const { return macro7Value.get(); }

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

int SuperChordPlugin::getProgressionValue() const {
    return static_cast<int>(progressionValue.get());
}

juce::String SuperChordPlugin::getCurrentPresetName() const {
    return VoicePresets::getPresetName(getCurrentVoicePresetIndex());
}

double SuperChordPlugin::calculateArpInterval(double /*bpm*/) const {
    // Rate values ordered slow to fast:
    // 0=2/1, 1=1/1, 2=1/1T, 3=1/2, 4=1/2T, 5=1/4, 6=1/4T, 7=1/8, 8=1/8T, 9=1/16, 10=1/16T, 11=1/32, 12=1/32T, 13=1/64
    // T = triplet (multiply by 2/3)
    static const double rateMultipliers[] = {
        8.0,                    // 0: 2/1 (2 whole notes)
        4.0,                    // 1: 1/1 (whole note)
        4.0 * 2.0 / 3.0,        // 2: 1/1T (whole triplet)
        2.0,                    // 3: 1/2 (half note)
        2.0 * 2.0 / 3.0,        // 4: 1/2T (half triplet)
        1.0,                    // 5: 1/4 (quarter note)
        1.0 * 2.0 / 3.0,        // 6: 1/4T (quarter triplet)
        0.5,                    // 7: 1/8 (eighth note)
        0.5 * 2.0 / 3.0,        // 8: 1/8T (eighth triplet)
        0.25,                   // 9: 1/16 (sixteenth note)
        0.25 * 2.0 / 3.0,       // 10: 1/16T (sixteenth triplet)
        0.125,                  // 11: 1/32 (thirty-second note)
        0.125 * 2.0 / 3.0,      // 12: 1/32T (thirty-second triplet)
        0.0625                  // 13: 1/64 (sixty-fourth note)
    };
    int rateIndex = getArpRateValue();
    rateIndex = juce::jlimit(0, 13, rateIndex);

    // Calculate PPQ interval (4 PPQ = 1 beat = 1/4 note)
    double beatsPerNote = rateMultipliers[rateIndex];
    return beatsPerNote;
}

void SuperChordPlugin::processNoteOn(int midiNote, float velocity,
                                     double currentPPQ, double bpm,
                                     int samplePosition) {
    // Get current progression type to determine available degrees
    auto progression = static_cast<ProgressionType>(getProgressionValue());
    int numDegrees = ChordEngine::getNumDegrees(progression);
    
    // Map MIDI notes to chord degrees
    // White keys map to degrees: C=0, D=1, E=2, F=3, G=4, A=5, B=6
    // Black keys map to the lower white key's degree
    // The octave of the played note determines the octave of the chord
    static const int noteToWhiteKey[] = {
        0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6  // C, C#, D, D#, E, F, F#, G, G#, A, A#, B
    };
    
    int noteInOctave = midiNote % 12;
    int whiteKeyIndex = noteToWhiteKey[noteInOctave];
    
    // For reduced degree progressions, map white keys to degrees and ignore extras
    int degree = whiteKeyIndex;
    
    // If degree is beyond available degrees, ignore this note
    if (degree >= numDegrees)
        return;
    
    currentChordDegree = degree;
    
    // Get the octave of the played note (MIDI note 60 = C4 = octave 4)
    // Octave offset relative to middle C (octave 4): C0=-4, C1=-3, C2=-2, C3=-1, C4=0, C5=+1, etc.
    int midiOctave = midiNote / 12;  // 0-10
    int octaveOffset = midiOctave - 5;  // Middle C (60) is in MIDI octave 5, make it offset 0

    // Get current parameters
    int key = getKeyValue();
    auto voicing = static_cast<ChordVoicing>(getVoicingValue());
    auto extension = static_cast<ChordExtension>(getExtensionValue());
    int octaveParam = getOctaveValue();  // Global octave shift from page 3
    int totalOctaveOffset = octaveOffset + octaveParam;
    int presetIndex = getCurrentVoicePresetIndex();

    // Generate chord with progression type
    juce::Array<int> chordNotes =
        chordEngine->generateChord(degree, key, voicing, extension, totalOctaveOffset, progression);
    currentChordName = chordEngine->getChordName(degree, key, extension, progression);

    // Store mapping from input MIDI note to generated chord notes
    midiNoteToChordNotes[midiNote] = chordNotes;
    
    // Update activeChordNotes for arpeggiator reference (uses most recent chord)
    activeChordNotes = chordNotes;

    // Update all voices with current preset and macro values
    for (int i = 0; i < synthesiser.getNumVoices(); i++) {
        if (auto *voice =
                dynamic_cast<SuperChordVoice *>(synthesiser.getVoice(i))) {
            voice->setPreset(presetIndex);
            voice->setMacro1(getMacro1Value());
            voice->setMacro2(getMacro2Value());
            voice->setMacro3(getMacro3Value());
            voice->setMacro4(getMacro4Value());
            voice->setMacro5(getMacro5Value());
            voice->setMacro6(getMacro6Value());
            voice->setMacro7(getMacro7Value());
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

    // Apply current pitch wheel value to newly triggered voices
    for (int i = 0; i < synthesiser.getNumVoices(); i++) {
        if (auto *voice =
                dynamic_cast<SuperChordVoice *>(synthesiser.getVoice(i))) {
            voice->setPitchBend(pitchWheelValue * 2.0f); // ±2 semitones
        }
    }
}

void SuperChordPlugin::processNoteOff(int midiNote, float velocity) {
    int arpMode = getArpModeValue();

    // Latch mode ignores note-off
    if (arpMode == 3)
        return;

    // Find the chord notes associated with this specific MIDI note
    auto it = midiNoteToChordNotes.find(midiNote);
    if (it != midiNoteToChordNotes.end()) {
        // Turn off only the chord notes for this MIDI note
        for (int note : it->second) {
            synthesiser.noteOff(1, note, velocity, true);
        }
        // Remove the mapping
        midiNoteToChordNotes.erase(it);
    }

    // Update activeChordNotes to reflect remaining active notes
    activeChordNotes.clear();
    for (const auto& pair : midiNoteToChordNotes) {
        for (int note : pair.second) {
            if (!activeChordNotes.contains(note)) {
                activeChordNotes.add(note);
            }
        }
    }

    // Reset arp step and chord display if no more notes are held
    if (midiNoteToChordNotes.empty()) {
        currentArpStep = 0;
        currentChordDegree = -1;
    }
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

// Arpeggiator direction patterns (1-indexed note positions)
// If a position exceeds numChordNotes, previous note is held (no new trigger)
static const int arpPatterns[][16] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},   // 0: Up
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},   // 1: Down
    {1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 6, 5, 4, 3, 2, 1},          // 2: Up-Down
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},          // 3: Random (handled specially)
    {1, 2, 5, 4, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},   // 4: Swing 1-2-5-4-3
    {4, 7, 1, 2, 5, 3, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16},   // 5: Jump 4-7-1-2
    {1, 3, 2, 4, 3, 5, 4, 6, 5, 7, 6, 8, 7, 9, 8, 10},         // 6: Stagger 1-3-2-4
    {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8},          // 7: Double 1-1-2-2
    {1, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9},          // 8: Pedal 1-2-1-3-1
    {1, 3, 5, 7, 2, 4, 6, 8, 1, 3, 5, 7, 2, 4, 6, 8},          // 9: Skip 1-3-5-7
    {8, 6, 4, 2, 7, 5, 3, 1, 8, 6, 4, 2, 7, 5, 3, 1},          // 10: Skip Down
    {1, 4, 2, 5, 3, 6, 4, 7, 5, 8, 6, 9, 7, 10, 8, 11},        // 11: Thirds 1-4-2-5
    {1, 5, 2, 6, 3, 7, 4, 8, 5, 9, 6, 10, 7, 11, 8, 12},       // 12: Fourths 1-5-2-6
    {1, 8, 2, 7, 3, 6, 4, 5, 1, 8, 2, 7, 3, 6, 4, 5},          // 13: Outside-In
    {4, 5, 3, 6, 2, 7, 1, 8, 4, 5, 3, 6, 2, 7, 1, 8},          // 14: Inside-Out
    {1, 2, 3, 1, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 8},          // 15: Wave
    {1, 1, 1, 2, 2, 3, 4, 5, 6, 7, 8, 8, 8, 7, 6, 5},          // 16: Accent
    {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3},          // 17: Pi (3.14159...)
    {1, 1, 2, 3, 5, 8, 5, 3, 2, 1, 1, 2, 3, 5, 8, 5},          // 18: Fibonacci
    {1, 4, 7, 2, 5, 8, 3, 6, 1, 4, 7, 2, 5, 8, 3, 6}           // 19: Circle
};
static const int numArpPatterns = 20;

void SuperChordPlugin::triggerArpStep(double /*currentPPQ*/, double /*bpm*/,
                                      int /*samplePosition*/) {
    if (activeChordNotes.isEmpty())
        return;

    int arpDirection = getArpDirectionValue();
    arpDirection = juce::jlimit(0, numArpPatterns - 1, arpDirection);
    int numChordNotes = activeChordNotes.size();
    int arpSteps = getArpStepsValue();
    int numSteps = juce::jmax(1, arpSteps);

    // Store previous note for note-off
    int prevPatternPos = arpPatterns[arpDirection][currentArpStep % 16];
    int prevNoteIndex = -1;
    if (arpDirection == 3) {
        // Random mode - previous step was random
        prevNoteIndex = currentArpStep % numChordNotes;
    } else if (prevPatternPos >= 1 && prevPatternPos <= numChordNotes) {
        prevNoteIndex = prevPatternPos - 1;
    }

    // Advance step
    currentArpStep = (currentArpStep + 1) % numSteps;

    // Get current note from pattern
    int currentNoteIndex = -1;
    if (arpDirection == 3) {
        // Random mode
        currentNoteIndex = juce::Random::getSystemRandom().nextInt(numChordNotes);
    } else {
        // Pattern-based: get position from pattern (1-indexed)
        int patternPos = arpPatterns[arpDirection][currentArpStep % 16];
        
        // If pattern position exceeds available notes, hold previous (don't trigger)
        if (patternPos >= 1 && patternPos <= numChordNotes) {
            currentNoteIndex = patternPos - 1;
        }
        // If patternPos > numChordNotes, currentNoteIndex stays -1 (no trigger, hold previous)
    }

    // Turn off previous note
    if (prevNoteIndex >= 0 && prevNoteIndex < numChordNotes) {
        synthesiser.noteOff(1, activeChordNotes[prevNoteIndex], 0.0f, true);
    }

    // Trigger current note (only if valid)
    if (currentNoteIndex >= 0 && currentNoteIndex < numChordNotes) {
        synthesiser.noteOn(1, activeChordNotes[currentNoteIndex], 0.8f);
        
        // Apply current pitch wheel value to newly triggered voice
        for (int i = 0; i < synthesiser.getNumVoices(); i++) {
            if (auto *voice =
                    dynamic_cast<SuperChordVoice *>(synthesiser.getVoice(i))) {
                voice->setPitchBend(pitchWheelValue * 2.0f); // ±2 semitones
            }
        }
    }
}

void SuperChordPlugin::applyToBuffer(const tracktion::PluginRenderContext &fc) {
    if (!isEnabled())
        return;

    // Get tempo info from the edit at the current edit time
    auto& tempo = edit.tempoSequence.getTempoAt(fc.editTime);
    double bpm = tempo.getBpm();

    // Handle all notes off (transport stopped, etc.)
    if (fc.bufferForMidiMessages != nullptr) {
        if (fc.bufferForMidiMessages->isAllNotesOff) {
            synthesiser.allNotesOff(0, false);
            activeChordNotes.clear();
            latchedNotes.clear();
            midiNoteToChordNotes.clear();
            currentArpStep = 0;
            currentChordDegree = -1;
        }
    }

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
            voice->setMacro4(getMacro4Value());
            voice->setMacro5(getMacro5Value());
            voice->setMacro6(getMacro6Value());
            voice->setMacro7(getMacro7Value());
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
        &macro3Value,      &macro4Value,       &macro5Value,
        &macro6Value,      &macro7Value,       &voicingValue,
        &octaveValue,      &keyValue,          &extensionValue,
        &arpModeValue,     &arpDirectionValue, &arpStepsValue,
        &arpRateValue,     nullptr};
    tracktion::copyPropertiesToNullTerminatedCachedValues(v, cvsFloat);

    for (auto p : getAutomatableParameters())
        p->updateFromAttachedValue();
}

} // namespace internal_plugins
