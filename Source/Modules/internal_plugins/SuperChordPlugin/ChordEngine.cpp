#include "ChordEngine.h"

namespace internal_plugins {

//==============================================================================
// Scale interval patterns (semitones from root)
const std::vector<int> ChordEngine::majorIntervals = {0, 2, 4, 5, 7, 9, 11};
const std::vector<int> ChordEngine::naturalMinorIntervals = {0, 2, 3, 5, 7, 8, 10};
const std::vector<int> ChordEngine::harmonicMinorIntervals = {0, 2, 3, 5, 7, 8, 11};
const std::vector<int> ChordEngine::melodicMinorIntervals = {0, 2, 3, 5, 7, 9, 11};
const std::vector<int> ChordEngine::dorianIntervals = {0, 2, 3, 5, 7, 9, 10};
const std::vector<int> ChordEngine::phrygianIntervals = {0, 1, 3, 5, 7, 8, 10};
const std::vector<int> ChordEngine::lydianIntervals = {0, 2, 4, 6, 7, 9, 11};
const std::vector<int> ChordEngine::mixolydianIntervals = {0, 2, 4, 5, 7, 9, 10};
const std::vector<int> ChordEngine::locrianIntervals = {0, 1, 3, 5, 6, 8, 10};

// Which degrees are minor in major scale (0=major, 1=minor)
// I=maj, ii=min, iii=min, IV=maj, V=maj, vi=min, vii°=dim
const std::vector<bool> ChordEngine::majorScaleMinorDegrees = {
    false, true, true, false, false, true, true};

// For natural minor: i=min, ii°=dim, III=maj, iv=min, v=min, VI=maj, VII=maj
const std::vector<bool> ChordEngine::minorScaleMinorDegrees = {
    true, true, false, true, true, false, false};

//==============================================================================
ChordEngine::ChordEngine() {}

//==============================================================================
const std::vector<int> &ChordEngine::getScaleIntervals(ScaleType scaleType) {
    switch (scaleType) {
    case ScaleType::Major:
        return majorIntervals;
    case ScaleType::NaturalMinor:
        return naturalMinorIntervals;
    case ScaleType::HarmonicMinor:
        return harmonicMinorIntervals;
    case ScaleType::MelodicMinor:
        return melodicMinorIntervals;
    case ScaleType::Dorian:
        return dorianIntervals;
    case ScaleType::Phrygian:
        return phrygianIntervals;
    case ScaleType::Lydian:
        return lydianIntervals;
    case ScaleType::Mixolydian:
        return mixolydianIntervals;
    case ScaleType::Locrian:
        return locrianIntervals;
    default:
        return majorIntervals;
    }
}

juce::StringArray ChordEngine::getScaleNames() {
    return {"Major",    "Natural Minor", "Harmonic Minor",
            "Melodic Minor", "Dorian",   "Phrygian",
            "Lydian",   "Mixolydian",    "Locrian"};
}

juce::StringArray ChordEngine::getVoicingNames() {
    return {"Close", "Open", "Drop-2", "Drop-3"};
}

//==============================================================================
int ChordEngine::getScaleDegree(int midiNote, int rootNote,
                                ScaleType scaleType) const {
    const auto &intervals = getScaleIntervals(scaleType);

    // Get note's position relative to root (0-11)
    int noteInOctave = (midiNote - rootNote + 120) % 12;

    // Find which scale degree this corresponds to
    for (int degree = 0; degree < 7; ++degree) {
        if (intervals[degree] == noteInOctave) {
            return degree + 1; // 1-indexed
        }
    }

    // If note isn't in scale, find closest degree
    int closestDegree = 1;
    int minDistance = 12;
    for (int degree = 0; degree < 7; ++degree) {
        int dist = std::abs(intervals[degree] - noteInOctave);
        if (dist < minDistance) {
            minDistance = dist;
            closestDegree = degree + 1;
        }
    }
    return closestDegree;
}

ChordEngine::ChordQuality
ChordEngine::getDiatonicQuality(int degree, ScaleType scaleType) const {
    // Base quality is always triad, pitch wheel morphs to others
    return ChordQuality::Triad;
}

//==============================================================================
std::vector<int> ChordEngine::getChordNotes(int triggerNote, int rootNote,
                                            ScaleType scaleType,
                                            float pitchWheelPos,
                                            VoicingType voicing) {
    const auto &intervals = getScaleIntervals(scaleType);

    // Determine which scale degree this trigger note represents
    int degree = getScaleDegree(triggerNote, rootNote, scaleType);
    int degreeIndex = degree - 1; // 0-indexed

    // Calculate the root of this chord (the scale degree note)
    int chordRootOctave = triggerNote / 12;
    int chordRootInOctave = rootNote + intervals[degreeIndex];
    int chordRoot = chordRootOctave * 12 + (chordRootInOctave % 12);

    // Adjust octave if chord root is higher than trigger
    while (chordRoot > triggerNote + 6)
        chordRoot -= 12;
    while (chordRoot < triggerNote - 6)
        chordRoot += 12;

    // Determine if this degree is minor
    bool isMinorScale =
        (scaleType == ScaleType::NaturalMinor ||
         scaleType == ScaleType::HarmonicMinor ||
         scaleType == ScaleType::MelodicMinor || scaleType == ScaleType::Dorian ||
         scaleType == ScaleType::Phrygian || scaleType == ScaleType::Locrian);

    const auto &minorDegrees =
        isMinorScale ? minorScaleMinorDegrees : majorScaleMinorDegrees;
    bool isMinorChord = minorDegrees[degreeIndex];

    // Build the chord with pitch wheel interpolation
    auto chordNotes = interpolateChordQuality(chordRoot, pitchWheelPos);

    // Adjust for minor if needed
    if (isMinorChord && chordNotes.size() > 1) {
        // Flatten the third
        chordNotes[1] = chordRoot + 3; // minor 3rd
    }

    // Apply voicing
    chordNotes = applyVoicing(chordNotes, voicing);

    // Update chord name
    lastRootMidi = chordRoot;
    lastWasMinor = isMinorChord;

    // Determine quality from pitch wheel
    if (pitchWheelPos < 0.25f) {
        lastQuality = ChordQuality::Sus2;
    } else if (pitchWheelPos < 0.5f) {
        lastQuality = ChordQuality::Triad;
    } else if (pitchWheelPos < 0.75f) {
        lastQuality = ChordQuality::Seventh;
    } else {
        lastQuality = ChordQuality::Ninth;
    }

    currentChordName =
        getNoteName(chordRoot) + getChordSuffix(lastQuality, isMinorChord);

    return chordNotes;
}

//==============================================================================
std::vector<int> ChordEngine::interpolateChordQuality(int rootMidi,
                                                      float pitchWheelPos) {
    std::vector<int> notes;
    notes.push_back(rootMidi); // Root always present

    // Pitch wheel zones with interpolation:
    // 0.0 - 0.25: Sus2 → Triad
    // 0.25 - 0.5: Triad (center approaching)
    // 0.5 - 0.75: Triad → 7th
    // 0.75 - 1.0: 7th → 9th

    if (pitchWheelPos < 0.125f) {
        // Pure sus2
        notes.push_back(rootMidi + 2);  // Major 2nd
        notes.push_back(rootMidi + 7);  // Perfect 5th
    } else if (pitchWheelPos < 0.375f) {
        // Transitioning sus2 to triad - use triad
        notes.push_back(rootMidi + 4);  // Major 3rd (will be adjusted for minor)
        notes.push_back(rootMidi + 7);  // Perfect 5th
    } else if (pitchWheelPos < 0.625f) {
        // Center zone - pure triad
        notes.push_back(rootMidi + 4);  // Major 3rd
        notes.push_back(rootMidi + 7);  // Perfect 5th
    } else if (pitchWheelPos < 0.875f) {
        // Transitioning to 7th
        notes.push_back(rootMidi + 4);  // Major 3rd
        notes.push_back(rootMidi + 7);  // Perfect 5th
        notes.push_back(rootMidi + 10); // Minor 7th (dominant)
    } else {
        // 9th chord
        notes.push_back(rootMidi + 4);  // Major 3rd
        notes.push_back(rootMidi + 7);  // Perfect 5th
        notes.push_back(rootMidi + 10); // Minor 7th
        notes.push_back(rootMidi + 14); // Major 9th
    }

    return notes;
}

//==============================================================================
std::vector<int> ChordEngine::applyVoicing(const std::vector<int> &notes,
                                           VoicingType voicing) {
    if (notes.size() < 3)
        return notes;

    std::vector<int> voiced = notes;

    switch (voicing) {
    case VoicingType::Close:
        // Already in close position
        break;

    case VoicingType::Open:
        // Spread notes across octaves
        if (voiced.size() >= 3) {
            voiced[1] += 12; // Move 3rd up an octave
        }
        if (voiced.size() >= 4) {
            voiced[3] += 12; // Move 7th up an octave
        }
        break;

    case VoicingType::Drop2:
        // Drop second note from top down an octave
        if (voiced.size() >= 3) {
            voiced[voiced.size() - 2] -= 12;
            std::sort(voiced.begin(), voiced.end());
        }
        break;

    case VoicingType::Drop3:
        // Drop third note from top down an octave
        if (voiced.size() >= 4) {
            voiced[voiced.size() - 3] -= 12;
            std::sort(voiced.begin(), voiced.end());
        }
        break;

    default:
        break;
    }

    return voiced;
}

//==============================================================================
juce::String ChordEngine::getNoteName(int midiNote) const {
    static const char *noteNames[] = {"C",  "C#", "D",  "D#", "E",  "F",
                                      "F#", "G",  "G#", "A",  "A#", "B"};
    return noteNames[midiNote % 12];
}

juce::String ChordEngine::getChordSuffix(ChordQuality quality,
                                         bool isMinor) const {
    switch (quality) {
    case ChordQuality::Sus2:
        return "sus2";
    case ChordQuality::Triad:
        return isMinor ? "m" : "";
    case ChordQuality::Seventh:
        return isMinor ? "m7" : "7";
    case ChordQuality::Ninth:
        return isMinor ? "m9" : "9";
    default:
        return "";
    }
}

} // namespace internal_plugins
