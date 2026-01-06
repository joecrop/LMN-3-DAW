#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace internal_plugins {

/**
 * SuperChordSound - Defines which notes the SuperChordVoice can play.
 * This sound accepts all MIDI notes since chord generation happens
 * in the plugin processor.
 */
class SuperChordSound : public juce::SynthesiserSound {
  public:
    SuperChordSound() = default;

    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }

    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

} // namespace internal_plugins
