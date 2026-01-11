#include "ChordEngine.h"

namespace internal_plugins {

const char *ChordEngine::noteNames[12] = {"C",  "C#", "D",  "D#", "E",  "F",
                                          "F#", "G",  "G#", "A",  "A#", "B"};

// Static progression configurations - 22 total
const ProgressionConfig ChordEngine::progressionConfigs[22] = {
    // 0: Diatonic Major - I, ii, iii, IV, V, vi, vii°
    {"Diatonic", 7, {0, 2, 4, 5, 7, 9, 11}, {0, 1, 1, 0, 0, 1, 2},
     {"I", "ii", "iii", "IV", "V", "vi", "vii"}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 1: Dorian - i, ii, III, IV, v, vi°, VII
    {"Dorian", 7, {0, 2, 3, 5, 7, 9, 10}, {1, 1, 0, 0, 1, 2, 0},
     {"i", "ii", "III", "IV", "v", "vi", "VII"}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 2: Mixolydian - I, ii, iii°, IV, v, vi, bVII
    {"Mixolydian", 7, {0, 2, 4, 5, 7, 9, 10}, {0, 1, 2, 0, 1, 1, 0},
     {"I", "ii", "iii", "IV", "v", "vi", "bVII"}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 3: Phrygian - i, bII, III, iv, v°, bVI, bVII
    {"Phrygian", 7, {0, 1, 3, 5, 7, 8, 10}, {1, 0, 0, 1, 2, 0, 0},
     {"i", "bII", "III", "iv", "v", "bVI", "bVII"}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 4: Lydian - I, II, iii, #iv°, V, vi, vii
    {"Lydian", 7, {0, 2, 4, 6, 7, 9, 11}, {0, 0, 1, 2, 0, 1, 1},
     {"I", "II", "iii", "#iv", "V", "vi", "vii"}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 5: Minor Subdominant (borrowed chords) - I, iv, bVI, bVII
    {"Borrowed", 4, {0, 5, 8, 10, 0, 0, 0}, {0, 1, 0, 0, 0, 0, 0},
     {"I", "iv", "bVI", "bVII", "", "", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 6: Blues - I7, IV7, V7
    {"Blues", 3, {0, 5, 7, 0, 0, 0, 0}, {4, 4, 4, 0, 0, 0, 0},
     {"I7", "IV7", "V7", "", "", "", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 7: Andalusian Cadence - i, bVII, bVI, V
    {"Andalusian", 4, {0, 10, 8, 7, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0},
     {"i", "bVII", "bVI", "V", "", "", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 8: Circle of Fifths - vi, ii, V, I, IV, vii°
    {"Circle", 6, {9, 2, 7, 0, 5, 11, 0}, {1, 1, 0, 0, 0, 2, 0},
     {"vi", "ii", "V", "I", "IV", "vii", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 9: Secondary Dominants - I, V/ii, V/iii, V/IV, V, V/vi, vii°
    {"Sec Dom", 7, {0, 9, 11, 0, 7, 4, 11}, {0, 4, 4, 4, 0, 4, 2},
     {"I", "V/ii", "V/iii", "V/IV", "V", "V/vi", "vii"}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 10: Tritone Substitution - I, bIII7, iii, bV7, V, bVII7, vii°
    {"Tritone", 7, {0, 3, 4, 6, 7, 10, 11}, {0, 4, 1, 4, 0, 4, 2},
     {"I", "bIII7", "iii", "bV7", "V", "bVII7", "vii"}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 11: Chromatic Mediants - I, bIII, III, bVI, VI, i
    {"Chr Med", 6, {0, 3, 4, 8, 9, 0, 0}, {0, 0, 0, 0, 0, 1, 0},
     {"I", "bIII", "III", "bVI", "VI", "i", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 12: Altered Dominants - I, ii, iii, IV, V7alt, vi, vii°
    {"Altered", 7, {0, 2, 4, 5, 7, 9, 11}, {0, 1, 1, 0, 4, 1, 2},
     {"I", "ii", "iii", "IV", "V7alt", "vi", "vii"}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 13: Coltrane Changes - Imaj7, bVImaj7, IIImaj7
    {"Coltrane", 3, {0, 8, 4, 0, 0, 0, 0}, {5, 5, 5, 0, 0, 0, 0},
     {"Imaj7", "bVImaj7", "IIImaj7", "", "", "", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 14: Omnibus - I, #I°7, ii, #ii°7
    {"Omnibus", 4, {0, 1, 2, 3, 0, 0, 0}, {0, 7, 1, 7, 0, 0, 0},
     {"I", "#I7", "ii", "#ii7", "", "", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 15: Quartal - stacked 4ths
    {"Quartal", 7, {0, 2, 4, 5, 7, 9, 11}, {0, 0, 0, 0, 0, 0, 0},
     {"Q1", "Q2", "Q3", "Q4", "Q5", "Q6", "Q7"}, true, false, false, {0,0,0,0,0,0,0}},
    
    // 16: Quintal - stacked 5ths
    {"Quintal", 7, {0, 2, 4, 5, 7, 9, 11}, {0, 0, 0, 0, 0, 0, 0},
     {"Q1", "Q2", "Q3", "Q4", "Q5", "Q6", "Q7"}, false, true, false, {0,0,0,0,0,0,0}},
    
    // 17: Chopin Chromatic - i, #i°, ii, #ii°, III
    {"Chopin", 5, {0, 1, 2, 3, 4, 0, 0}, {1, 2, 1, 2, 0, 0, 0},
     {"i", "#i", "ii", "#ii", "III", "", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 18: Diminished Descent - i, vii°, bvii°, vi°
    {"Dim Desc", 4, {0, 11, 10, 9, 0, 0, 0}, {1, 2, 2, 2, 0, 0, 0},
     {"i", "vii", "bvii", "vi", "", "", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 19: Major III Surprise - i, III, iv, V
    {"III Surp", 4, {0, 4, 5, 7, 0, 0, 0}, {1, 0, 1, 0, 0, 0, 0},
     {"i", "III", "iv", "V", "", "", ""}, false, false, false, {0,0,0,0,0,0,0}},
    
    // 20: Chromatic Bass - I with descending chromatic bass
    {"Chr Bass", 6, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0},
     {"I", "I/7", "I/b7", "I/6", "I/b6", "I/5", ""}, false, false, true, {0, -1, -2, -3, -4, -5, 0}},
    
    // 21: 7-3-6 Gospel - vii°, iii, vi, ii
    {"Gospel", 4, {11, 4, 9, 2, 0, 0, 0}, {2, 1, 1, 1, 0, 0, 0},
     {"vii", "iii", "vi", "ii", "", "", ""}, false, false, false, {0,0,0,0,0,0,0}},
};

int ChordEngine::getScaleNote(int key, int degree, ProgressionType progression) {
    const auto& config = progressionConfigs[static_cast<int>(progression)];
    int clampedDegree = juce::jlimit(0, config.numDegrees - 1, degree);
    int interval = config.scaleIntervals[clampedDegree];
    // Return the full semitone offset including octave wrap
    // This preserves octave information when key + interval >= 12
    return key + interval;
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
    case 3: // Augmented
        intervals.add(0);
        intervals.add(4);
        intervals.add(8);
        break;
    case 4: // Dominant 7 (for blues etc) - triad only
        intervals.add(0);
        intervals.add(4);
        intervals.add(7);
        break;
    case 5: // Major 7 triad base
        intervals.add(0);
        intervals.add(4);
        intervals.add(7);
        break;
    case 6: // Minor 7 triad base
        intervals.add(0);
        intervals.add(3);
        intervals.add(7);
        break;
    case 7: // Diminished 7 triad base
        intervals.add(0);
        intervals.add(3);
        intervals.add(6);
        break;
    case 8: // Half-diminished triad base
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
    case 0: // Major -> Major 7
        intervals.add(11);
        break;
    case 1: // Minor -> Minor 7
        intervals.add(10);
        break;
    case 2: // Diminished -> Half-diminished 7
        intervals.add(10);
        break;
    case 3: // Augmented -> Augmented Major 7
        intervals.add(11);
        break;
    case 4: // Dominant 7
        intervals.add(10);
        break;
    case 5: // Major 7
        intervals.add(11);
        break;
    case 6: // Minor 7
        intervals.add(10);
        break;
    case 7: // Diminished 7 (full diminished)
        intervals.add(9);
        break;
    case 8: // Half-diminished 7
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

juce::Array<int> ChordEngine::getQuartalIntervals() {
    // Stack of perfect 4ths: root, P4, P4, P4
    juce::Array<int> intervals;
    intervals.add(0);
    intervals.add(5);
    intervals.add(10);
    intervals.add(15);
    return intervals;
}

juce::Array<int> ChordEngine::getQuintalIntervals() {
    // Stack of perfect 5ths: root, P5, P5, P5
    juce::Array<int> intervals;
    intervals.add(0);
    intervals.add(7);
    intervals.add(14);
    intervals.add(21);
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
        voiced = notes;
        break;

    case ChordVoicing::Open:
        voiced.add(notes[0]);
        voiced.add(notes[1] + 12);
        if (notes.size() > 2)
            voiced.add(notes[2]);
        for (int i = 3; i < notes.size(); i++) {
            voiced.add(notes[i]);
        }
        break;

    case ChordVoicing::Drop2:
        if (notes.size() >= 3) {
            voiced.add(notes[0]);
            voiced.add(notes.getLast());
            voiced.add(notes[1]);
            if (notes.size() > 3) {
                voiced.add(notes[notes.size() - 2] - 12);
            }
            if (notes.size() > 2 && notes.size() <= 3) {
                voiced.add(notes[2]);
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
                                            int octaveOffset,
                                            ProgressionType progression) {
    const auto& config = progressionConfigs[static_cast<int>(progression)];
    
    // Validate inputs
    degree = juce::jlimit(0, config.numDegrees - 1, degree);
    key = juce::jlimit(0, 11, key);
    octaveOffset = juce::jlimit(-3, 2, octaveOffset);  // Allow wider octave range

    // Get root note of the chord
    int rootNote = getScaleNote(key, degree, progression);

    // Get chord quality for this degree
    int quality = config.chordQualities[degree];

    // Get intervals based on voicing type and extension
    juce::Array<int> intervals;
    
    if (config.useQuartalVoicing) {
        intervals = getQuartalIntervals();
    } else if (config.useQuintalVoicing) {
        intervals = getQuintalIntervals();
    } else {
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
    }

    // Build chord notes from root
    int baseNote = 60 + rootNote + (octaveOffset * 12);
    juce::Array<int> notes;

    for (int interval : intervals) {
        notes.add(baseNote + interval);
    }

    // Handle chromatic bass mode
    if (config.chromaticBass) {
        // Move bass note chromatically
        int bassOffset = config.bassOffsets[degree];
        if (!notes.isEmpty()) {
            notes.set(0, notes[0] + bassOffset);
        }
    }

    // Apply voicing (skip for quartal/quintal which have their own voicing)
    if (!config.useQuartalVoicing && !config.useQuintalVoicing) {
        notes = applyVoicing(notes, voicing);
    }

    return notes;
}

juce::String ChordEngine::getChordName(int degree, int key,
                                       ChordExtension extension,
                                       ProgressionType progression) {
    const auto& config = progressionConfigs[static_cast<int>(progression)];
    degree = juce::jlimit(0, config.numDegrees - 1, degree);
    key = juce::jlimit(0, 11, key);

    int rootNote = getScaleNote(key, degree, progression);
    int quality = config.chordQualities[degree];

    // Use modulo 12 for note name lookup (octave doesn't affect the name)
    juce::String name = noteNames[rootNote % 12];

    // Handle quartal/quintal specially
    if (config.useQuartalVoicing) {
        name += "sus4";
        return name + " - " + config.degreeLabels[degree];
    }
    if (config.useQuintalVoicing) {
        name += "5";
        return name + " - " + config.degreeLabels[degree];
    }

    // Add quality suffix based on quality code
    switch (quality) {
    case 0: // Major
        if (extension == ChordExtension::Seventh)
            name += "maj7";
        else if (extension == ChordExtension::Ninth)
            name += "maj9";
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
            name += "7";
        else if (extension == ChordExtension::Ninth)
            name += "9";
        break;
    case 3: // Augmented
        name += "aug";
        if (extension == ChordExtension::Seventh)
            name += "7";
        break;
    case 4: // Dominant 7
        name += "7";
        if (extension == ChordExtension::Ninth)
            name = noteNames[rootNote] + juce::String("9");
        break;
    case 5: // Major 7
        name += "maj7";
        if (extension == ChordExtension::Ninth)
            name = noteNames[rootNote] + juce::String("maj9");
        break;
    case 6: // Minor 7
        name += "m7";
        if (extension == ChordExtension::Ninth)
            name = noteNames[rootNote] + juce::String("m9");
        break;
    case 7: // Diminished 7
        name += "dim7";
        break;
    case 8: // Half-diminished
        name += "m7b5";
        break;
    }

    // Add degree label
    name += " - " + juce::String(config.degreeLabels[degree]);

    return name;
}

juce::String ChordEngine::getDegreeLabel(ProgressionType progression, int degree) {
    const auto& config = progressionConfigs[static_cast<int>(progression)];
    if (degree >= 0 && degree < config.numDegrees) {
        return config.degreeLabels[degree];
    }
    return "?";
}

int ChordEngine::getNumDegrees(ProgressionType progression) {
    return progressionConfigs[static_cast<int>(progression)].numDegrees;
}

juce::String ChordEngine::getProgressionName(ProgressionType progression) {
    return progressionConfigs[static_cast<int>(progression)].name;
}

const ProgressionConfig& ChordEngine::getProgressionConfig(ProgressionType progression) {
    return progressionConfigs[static_cast<int>(progression)];
}

} // namespace internal_plugins
