#include "ChordEngine.h"

namespace internal_plugins {

const char *ChordEngine::noteNames[12] = {"C",  "C#", "D",  "D#", "E",  "F",
                                          "F#", "G",  "G#", "A",  "A#", "B"};

int ChordEngine::getMajorScaleNote(int key, int degree) {
    // Get the MIDI note for a degree in the major scale starting from key
    // Key: 0=C, 1=C#, ... 11=B
    // Degree: 0-6 (I-VII)
    int interval = majorScaleIntervals[degree % 7];
    return (key + interval) % 12;
}

juce::Array<int> ChordEngine::getTriadIntervals(int quality) {
    juce::Array<int> intervals;

    switch (quality) {
    case 0: // Major
        intervals.add(0);
        intervals.add(4);
        intervals.add(7);
        break;
    case 1: // Minor
        intervals.add(0);
        intervals.add(3);
        intervals.add(7);
        break;
    case 2: // Diminished
        intervals.add(0);
        intervals.add(3);
        intervals.add(6);
        break;
    default:
        intervals.add(0);
        intervals.add(4);
        intervals.add(7);
        break;
    }

    return intervals;
}

juce::Array<int> ChordEngine::getSeventhIntervals(int quality) {
    juce::Array<int> intervals = getTriadIntervals(quality);

    switch (quality) {
    case 0: // Major 7
        intervals.add(11);
        break;
    case 1: // Minor 7
        intervals.add(10);
        break;
    case 2: // Diminished 7 (half-diminished)
        intervals.add(10);
        break;
    default:
        intervals.add(10);
        break;
    }

    return intervals;
}

juce::Array<int> ChordEngine::getNinthIntervals(int quality) {
    juce::Array<int> intervals = getSeventhIntervals(quality);
    intervals.add(14); // 9th is always a major 2nd above octave
    return intervals;
}

juce::Array<int> ChordEngine::applyVoicing(const juce::Array<int> &notes,
                                           ChordVoicing voicing) {
    if (notes.size() < 3) {
        return notes;
    }

    juce::Array<int> voiced;

    switch (voicing) {
    case ChordVoicing::Close:
        // All notes within one octave, as-is
        voiced = notes;
        break;

    case ChordVoicing::Open:
        // Spread root and 5th by an octave
        voiced.add(notes[0]);      // Root stays
        voiced.add(notes[1] + 12); // 3rd up an octave
        if (notes.size() > 2)
            voiced.add(notes[2]); // 5th stays
        for (int i = 3; i < notes.size(); i++) {
            voiced.add(notes[i]);
        }
        break;

    case ChordVoicing::Drop2:
        // Drop the second-highest note by an octave
        if (notes.size() >= 3) {
            voiced.add(notes[0]);           // Root
            voiced.add(notes.getLast());    // Highest note
            voiced.add(notes[1]);           // 3rd
            if (notes.size() > 3) {
                // Drop second-highest
                voiced.add(notes[notes.size() - 2] - 12);
            }
            if (notes.size() > 2 && notes.size() <= 3) {
                voiced.add(notes[2]); // 5th
            }
        } else {
            voiced = notes;
        }
        break;
    }

    return voiced;
}

juce::Array<int> ChordEngine::generateChord(int degree, int key,
                                            ChordVoicing voicing,
                                            ChordExtension extension,
                                            int octaveOffset) {
    // Validate inputs
    degree = juce::jlimit(0, 6, degree);
    key = juce::jlimit(0, 11, key);
    octaveOffset = juce::jlimit(-1, 1, octaveOffset);

    // Get root note of the chord
    int rootNote = getMajorScaleNote(key, degree);

    // Get chord quality for this degree
    int quality = chordQualities[degree];

    // Get intervals based on extension
    juce::Array<int> intervals;
    switch (extension) {
    case ChordExtension::Triad:
        intervals = getTriadIntervals(quality);
        break;
    case ChordExtension::Seventh:
        intervals = getSeventhIntervals(quality);
        break;
    case ChordExtension::Ninth:
        intervals = getNinthIntervals(quality);
        break;
    }

    // Build chord notes from root
    // Base octave is C4 (MIDI note 60)
    int baseNote = 60 + rootNote + (octaveOffset * 12);
    juce::Array<int> notes;

    for (int interval : intervals) {
        notes.add(baseNote + interval);
    }

    // Apply voicing
    notes = applyVoicing(notes, voicing);

    return notes;
}

juce::String ChordEngine::getChordName(int degree, int key,
                                       ChordExtension extension) {
    degree = juce::jlimit(0, 6, degree);
    key = juce::jlimit(0, 11, key);

    int rootNote = getMajorScaleNote(key, degree);
    int quality = chordQualities[degree];

    juce::String name = noteNames[rootNote];

    // Add quality suffix
    switch (quality) {
    case 0: // Major
        if (extension == ChordExtension::Seventh)
            name += "maj7";
        else if (extension == ChordExtension::Ninth)
            name += "maj9";
        // Triads don't need suffix for major
        break;
    case 1: // Minor
        name += "m";
        if (extension == ChordExtension::Seventh)
            name += "7";
        else if (extension == ChordExtension::Ninth)
            name += "9";
        break;
    case 2: // Diminished
        name += "dim";
        if (extension == ChordExtension::Seventh)
            name += "7"; // Half-diminished
        else if (extension == ChordExtension::Ninth)
            name += "9";
        break;
    }

    // Add Roman numeral
    name += " - " + getDegreeRomanNumeral(degree);

    return name;
}

juce::String ChordEngine::getDegreeRomanNumeral(int degree) {
    static const char *numerals[] = {"I", "ii", "iii", "IV", "V", "vi", "vii°"};
    if (degree >= 0 && degree < 7) {
        return numerals[degree];
    }
    return "?";
}

} // namespace internal_plugins
