#include "SuperChordPlugin.h"

namespace internal_plugins {

// Custom identifiers for SuperChord-specific state
namespace SuperChordIDs {
static const juce::Identifier warmth{"warmth"};
static const juce::Identifier space{"space"};
static const juce::Identifier scAttack{"scAttack"};
static const juce::Identifier bloom{"bloom"};
static const juce::Identifier voicePreset{"voicePreset"};
} // namespace SuperChordIDs

const char *SuperChordPlugin::xmlTypeName = "superChord";

//==============================================================================
SuperChordPlugin::SuperChordPlugin(tracktion::PluginCreationInfo info)
    : Plugin(info), chordEngine(), voicePresets() {
    auto um = getUndoManager();

    // Initialize macro parameters with custom IDs
    warmthValue.referTo(state, SuperChordIDs::warmth, um, 0.5f);
    spaceValue.referTo(state, SuperChordIDs::space, um, 0.3f);
    attackValue.referTo(state, SuperChordIDs::scAttack, um, 0.4f);
    bloomValue.referTo(state, SuperChordIDs::bloom, um, 0.5f);

    // Create automatable parameters
    warmthParam = addParam("warmth", TRANS("Warmth"), {0.0f, 1.0f});
    spaceParam = addParam("space", TRANS("Space"), {0.0f, 1.0f});
    attackParam = addParam("attack", TRANS("Attack"), {0.0f, 1.0f});
    bloomParam = addParam("bloom", TRANS("Bloom"), {0.0f, 1.0f});

    warmthParam->attachToCurrentValue(warmthValue);
    spaceParam->attachToCurrentValue(spaceValue);
    attackParam->attachToCurrentValue(attackValue);
    bloomParam->attachToCurrentValue(bloomValue);

    // Musical settings
    rootNoteValue.referTo(state, tracktion::IDs::root, um, 0);      // C
    scaleTypeValue.referTo(state, tracktion::IDs::scale, um, 0);    // Major
    voicePresetValue.referTo(state, SuperChordIDs::voicePreset, um, 0); // Ethereal
    playModeValue.referTo(state, tracktion::IDs::mode, um, 0);      // Chord
    playSubModeValue.referTo(state, tracktion::IDs::type, um, 0);   // Close

    // Add synth voices (enough for 4 chords * 7 notes max)
    for (int i = 0; i < 28; ++i) {
        synthesiser.addVoice(new SuperChordVoice(*this));
    }

    // Add the sound that all voices can play
    synthesiser.addSound(new SuperChordSound());
}

SuperChordPlugin::~SuperChordPlugin() {
    notifyListenersOfDeletion();

    warmthParam->detachFromCurrentValue();
    spaceParam->detachFromCurrentValue();
    attackParam->detachFromCurrentValue();
    bloomParam->detachFromCurrentValue();
}

//==============================================================================
void SuperChordPlugin::initialise(const tracktion::PluginInitialisationInfo &info) {
    sampleRate = info.sampleRate;
    blockSize = info.blockSizeSamples;
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
}

void SuperChordPlugin::deinitialise() { synthesiser.allNotesOff(0, false); }

void SuperChordPlugin::reset() { synthesiser.allNotesOff(0, true); }

double SuperChordPlugin::getTailLength() const {
    // Return release time based on bloom parameter
    return 0.1 + bloomValue.get() * 5.0; // 0.1s to 5.1s
}

//==============================================================================
void SuperChordPlugin::applyToBuffer(const tracktion::PluginRenderContext &fc) {
    if (!isEnabled())
        return;

    // Update synth parameters before processing
    updateSynthParameters();

    // Process MIDI messages
    if (fc.bufferForMidiMessages != nullptr) {
        for (auto &midiMessage : *fc.bufferForMidiMessages) {
            // MidiMessageWithSource inherits from juce::MidiMessage
            const juce::MidiMessage &msg = midiMessage;

            if (msg.isNoteOn()) {
                processNoteOn(msg.getNoteNumber(), msg.getVelocity() / 127.0f);
            } else if (msg.isNoteOff()) {
                processNoteOff(msg.getNoteNumber());
            } else if (msg.isPitchWheel()) {
                processPitchWheel(msg.getPitchWheelValue());
            }
        }
    }

    // Create a MIDI buffer for the synthesiser
    juce::MidiBuffer midiBuffer;

    // Render audio
    synthesiser.renderNextBlock(*fc.destBuffer, midiBuffer, fc.bufferStartSample,
                                fc.bufferNumSamples);
}

//==============================================================================
void SuperChordPlugin::processNoteOn(int noteNumber, float velocity) {
    // Get the chord notes for this trigger note
    auto chordNotes = chordEngine.getChordNotes(
        noteNumber, rootNoteValue.get(),
        static_cast<ChordEngine::ScaleType>(scaleTypeValue.get()),
        currentPitchWheel,
        static_cast<ChordEngine::VoicingType>(playSubModeValue.get()));

    // Store active notes for this trigger
    activeChordNotes[noteNumber] = chordNotes;

    // Calculate strum delay if in strum mode
    int strumDelayMs = 0;
    bool isStrumMode = playModeValue.get() == 1; // Strum mode
    int strumDirection = playSubModeValue.get(); // 0=up, 1=down, 2=alternate

    // Trigger each note in the chord
    for (size_t i = 0; i < chordNotes.size(); ++i) {
        int chordNote = chordNotes[i];

        if (isStrumMode) {
            // Calculate delay based on position and direction
            int noteIndex = (strumDirection == 1)
                                ? (int)(chordNotes.size() - 1 - i)
                                : (int)i;
            strumDelayMs = noteIndex * 20; // 20ms between notes
        }

        // For now, trigger immediately (strum delay would need timer)
        synthesiser.noteOn(1, chordNote, velocity);
    }

    notesPlaying++;
    lastNoteVelocity = velocity;
    triggerAsyncUpdate();
}

void SuperChordPlugin::processNoteOff(int noteNumber) {
    // Find and release all notes in this chord
    auto it = activeChordNotes.find(noteNumber);
    if (it != activeChordNotes.end()) {
        for (int chordNote : it->second) {
            synthesiser.noteOff(1, chordNote, 0.0f, true);
        }
        activeChordNotes.erase(it);
        notesPlaying = juce::jmax(0, notesPlaying - 1);
    }
    triggerAsyncUpdate();
}

void SuperChordPlugin::processPitchWheel(int wheelValue) {
    // Convert 0-16383 to 0.0-1.0
    currentPitchWheel = wheelValue / 16383.0f;

    // Update active chords with new quality
    // This would require retriggering voices with morphed chord
    // For now, store for next note
    triggerAsyncUpdate();
}

void SuperChordPlugin::updateSynthParameters() {
    // Update all voices with current macro values
    for (int i = 0; i < synthesiser.getNumVoices(); ++i) {
        if (auto *voice =
                dynamic_cast<SuperChordVoice *>(synthesiser.getVoice(i))) {
            voice->updateParameters(warmthValue.get(), spaceValue.get(),
                                    attackValue.get(), bloomValue.get(),
                                    voicePresets.getPreset(voicePresetValue.get()));
        }
    }
}

//==============================================================================
juce::String SuperChordPlugin::getCurrentChordName() const {
    return chordEngine.getCurrentChordName();
}

void SuperChordPlugin::handleAsyncUpdate() {
    listeners.call(&Listener::chordChanged);
    listeners.call(&Listener::parameterChanged);
    if (lastNoteVelocity > 0.0f) {
        listeners.call(&Listener::noteTriggered, lastNoteVelocity);
    }
}

//==============================================================================
void SuperChordPlugin::restorePluginStateFromValueTree(
    const juce::ValueTree &v) {
    juce::CachedValue<float> *cvsFloat[] = {&warmthValue, &spaceValue,
                                            &attackValue, &bloomValue, nullptr};
    juce::CachedValue<int> *cvsInt[] = {&rootNoteValue, &scaleTypeValue,
                                        &voicePresetValue, &playModeValue,
                                        &playSubModeValue, nullptr};

    tracktion::copyPropertiesToNullTerminatedCachedValues(v, cvsFloat);
    tracktion::copyPropertiesToNullTerminatedCachedValues(v, cvsInt);

    for (auto p : getAutomatableParameters())
        p->updateFromAttachedValue();
}

} // namespace internal_plugins
