#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace internal_plugins {

/**
 * Chord voicing types
 */
enum class ChordVoicing { Close = 0, Open = 1, Drop2 = 2 };

/**
 * Chord extension types
 */
enum class ChordExtension { Triad = 0, Seventh = 1, Ninth = 2 };

/**
 * ChordEngine - Generates diatonic chords using the Nashville Number System.
 * Supports I-VII degrees in any major key with various voicings and extensions.
 */
class ChordEngine {
  public:
    ChordEngine() = default;
    ~ChordEngine() = default;

    /**
     * Generate chord notes based on degree, key, voicing, and extension.
     *
     * @param degree Chord degree (0-6 for I-VII)
     * @param key Key signature (0-11 for C-B)
     * @param voicing Chord voicing type
     * @param extension Chord extension type
     * @param octaveOffset Octave offset (-1, 0, +1)
     * @return Array of MIDI note numbers
     */
    juce::Array<int> generateChord(int degree, int key, ChordVoicing voicing,
                                   ChordExtension extension,
                                   int octaveOffset = 0);

    /**
     * Get the chord quality name for display
     *
     * @param degree Chord degree (0-6 for I-VII)
     * @param key Key signature (0-11 for C-B)
     * @param extension Chord extension type
     * @return Chord name string (e.g., "C Major", "Am7")
     */
    juce::String getChordName(int degree, int key, ChordExtension extension);

    /**
     * Get the Roman numeral for a degree
     */
    static juce::String getDegreeRomanNumeral(int degree);

  private:
    // Major scale intervals (semitones from root)
    static constexpr int majorScaleIntervals[7] = {0, 2, 4, 5, 7, 9, 11};

    // Chord quality for each degree in major scale
    // 0 = major, 1 = minor, 2 = diminished
    static constexpr int chordQualities[7] = {0, 1, 1, 0, 0, 1, 2};

    // Note names
    static const char *noteNames[12];

    int getMajorScaleNote(int key, int degree);
    juce::Array<int> getTriadIntervals(int quality);
    juce::Array<int> getSeventhIntervals(int quality);
    juce::Array<int> getNinthIntervals(int quality);
    juce::Array<int> applyVoicing(const juce::Array<int> &notes,
                                   ChordVoicing voicing);
};

} // namespace internal_plugins
