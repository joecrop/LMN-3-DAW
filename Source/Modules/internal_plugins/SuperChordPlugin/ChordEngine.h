#pragma once

#include <juce_core/juce_core.h>
#include <tracktion_engine/tracktion_engine.h>
#include <vector>

namespace internal_plugins {

/**
 * ChordEngine - Handles diatonic chord generation and pitch wheel morphing.
 *
 * Converts single MIDI notes into chord note arrays based on:
 * - Root note and scale type
 * - Diatonic degree (I-VII) from input note
 * - Pitch wheel position for chord quality morphing
 * - Voicing type (close, open, drop-2, drop-3)
 */
class ChordEngine {
  public:
    //==========================================================================
    // Scale types
    enum class ScaleType {
        Major = 0,
        NaturalMinor,
        HarmonicMinor,
        MelodicMinor,
        Dorian,
        Phrygian,
        Lydian,
        Mixolydian,
        Locrian,
        NumScales
    };

    //==========================================================================
    // Chord quality types (for pitch wheel morphing)
    enum class ChordQuality {
        Sus2 = 0,
        Triad,
        Seventh,
        Ninth
    };

    //==========================================================================
    // Voicing types
    enum class VoicingType {
        Close = 0,   // All notes in closest position
        Open,        // Spread across octaves
        Drop2,       // Second note from top dropped an octave
        Drop3,       // Third note from top dropped an octave
        NumVoicings
    };

    //==========================================================================
    ChordEngine();

    /**
     * Get chord notes for a given trigger note.
     *
     * @param triggerNote The MIDI note number that triggered the chord
     * @param rootNote The root note of the key (0-11, C=0)
     * @param scaleType The scale type to use
     * @param pitchWheelPos Pitch wheel position (0.0-1.0, 0.5=center)
     * @param voicing The voicing type to apply
     * @return Vector of MIDI note numbers forming the chord
     */
    std::vector<int> getChordNotes(int triggerNote, int rootNote,
                                   ScaleType scaleType, float pitchWheelPos,
                                   VoicingType voicing);

    /**
     * Get the current chord name (e.g., "Cmaj7", "Dm", "G9")
     */
    juce::String getCurrentChordName() const { return currentChordName; }

    /**
     * Get scale degree (1-7) for a note in a given scale
     */
    int getScaleDegree(int midiNote, int rootNote, ScaleType scaleType) const;

    /**
     * Get diatonic chord quality for a scale degree
     */
    ChordQuality getDiatonicQuality(int degree, ScaleType scaleType) const;

    //==========================================================================
    // Static utilities
    static juce::StringArray getScaleNames();
    static juce::StringArray getVoicingNames();
    static const std::vector<int> &getScaleIntervals(ScaleType scaleType);

  private:
    // Build chord from degree and quality
    std::vector<int> buildChord(int rootMidi, ChordQuality quality,
                                float pitchWheelPos);

    // Apply voicing transformation
    std::vector<int> applyVoicing(const std::vector<int> &notes,
                                  VoicingType voicing);

    // Interpolate between chord qualities based on pitch wheel
    std::vector<int> interpolateChordQuality(int rootMidi, float pitchWheelPos);

    // Get note name for chord naming
    juce::String getNoteName(int midiNote) const;

    // Get chord suffix for quality
    juce::String getChordSuffix(ChordQuality quality, bool isMinor) const;

    juce::String currentChordName;
    int lastRootMidi = 60;
    ChordQuality lastQuality = ChordQuality::Triad;
    bool lastWasMinor = false;

    // Scale interval patterns (semitones from root)
    static const std::vector<int> majorIntervals;
    static const std::vector<int> naturalMinorIntervals;
    static const std::vector<int> harmonicMinorIntervals;
    static const std::vector<int> melodicMinorIntervals;
    static const std::vector<int> dorianIntervals;
    static const std::vector<int> phrygianIntervals;
    static const std::vector<int> lydianIntervals;
    static const std::vector<int> mixolydianIntervals;
    static const std::vector<int> locrianIntervals;

    // Diatonic chord qualities for major scale (I=maj, ii=min, etc.)
    static const std::vector<bool> majorScaleMinorDegrees;
    static const std::vector<bool> minorScaleMinorDegrees;
};

} // namespace internal_plugins
