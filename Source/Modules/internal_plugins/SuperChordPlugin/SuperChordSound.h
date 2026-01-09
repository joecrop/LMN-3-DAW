#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace internal_plugins {

/**
 * SuperChordSound - SynthesiserSound for SuperChord plugin.
 * This is a simple sound class that applies to all notes and channels.
 */
class SuperChordSound : public juce::SynthesiserSound {
  public:
    SuperChordSound() = default;
    ~SuperChordSound() override = default;

    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

} // namespace internal_plugins
