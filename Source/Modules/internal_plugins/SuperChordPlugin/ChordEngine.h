#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "SuperChordPlugin.h"

namespace internal_plugins {

/**
 * Chord voicing types - 20 total voicing options
 */
enum class ChordVoicing {
    // Basic Voicings (0-3)
    Block = 0,      // All notes at base position
    Drop2 = 1,      // 2nd from top dropped an octave
    Drop3 = 2,      // 3rd from top dropped an octave
    Spread = 3,     // Alternating octave displacement
    
    // Inversions (4-6)
    Inv1 = 4,       // 1st inversion - bass up octave
    Inv2 = 5,       // 2nd inversion - two lowest up octave
    Inv3 = 6,       // 3rd inversion - three lowest up octave
    
    // Double Drops (7-8)
    Drop24 = 7,     // Drop 2nd and 4th from top
    Drop23 = 8,     // Drop 2nd and 3rd from top
    
    // Range & Spacing (9-12)
    Close = 9,      // Notes within one octave (compact)
    Open = 10,      // Every other note up octave
    Wide = 11,      // Root down, 5th up octave
    Cluster = 12,   // Tight semitone clustering
    
    // Doubling (13-16)
    DblRoot = 13,   // Root doubled octave below
    Dbl5th = 14,    // 5th doubled octave above
    Octaves = 15,   // All notes doubled octave above
    Power = 16,     // Root + 5th only
    
    // Style-Specific (17-19)
    Shell = 17,     // Root + 3rd + 7th only
    Quartal = 18,   // Restack in 4ths
    Random = 19     // Random octave displacement
};

/**
 * Chord extension types
 */
enum class ChordExtension { Triad = 0, Seventh = 1, Ninth = 2 };

/**
 * Configuration for a single progression type
 */
struct ProgressionConfig {
    const char* name;           // Display name
    int numDegrees;             // Number of available chord degrees (3-7)
    int scaleIntervals[7];      // Scale intervals for each degree (semitones from root)
    int chordQualities[7];      // 0=major, 1=minor, 2=dim, 3=aug, 4=dom7, 5=maj7, 6=min7, 7=dim7, 8=hdim7
    const char* degreeLabels[7]; // Roman numeral labels for display
    bool useQuartalVoicing;     // Stack 4ths instead of 3rds
    bool useQuintalVoicing;     // Stack 5ths instead of 3rds
    bool chromaticBass;         // Use chromatic descending bass
    int bassOffsets[7];         // Bass note offsets for chromatic bass mode
};

/**
 * ChordEngine - Generates chords using various progression types.
 * Supports 22 different progression modes with dynamic degree counts.
 */
class ChordEngine {
  public:
    ChordEngine() = default;
    ~ChordEngine() = default;

    /**
     * Generate chord notes based on degree, key, voicing, extension, and progression.
     */
    juce::Array<int> generateChord(int degree, int key, ChordVoicing voicing,
                                   ChordExtension extension, int octaveOffset,
                                   ProgressionType progression);
    
    // Legacy method for backwards compatibility
    juce::Array<int> generateChord(int degree, int key, ChordVoicing voicing,
                                   ChordExtension extension,
                                   int octaveOffset = 0) {
        return generateChord(degree, key, voicing, extension, octaveOffset, 
                             ProgressionType::DiatonicMajor);
    }

    /**
     * Get the chord quality name for display
     */
    juce::String getChordName(int degree, int key, ChordExtension extension,
                              ProgressionType progression);
    
    // Legacy method
    juce::String getChordName(int degree, int key, ChordExtension extension) {
        return getChordName(degree, key, extension, ProgressionType::DiatonicMajor);
    }

    /**
     * Get the Roman numeral for a degree in a given progression
     */
    static juce::String getDegreeLabel(ProgressionType progression, int degree);

    /**
     * Get the number of available degrees for a progression
     */
    static int getNumDegrees(ProgressionType progression);

    /**
     * Get the progression display name
     */
    static juce::String getProgressionName(ProgressionType progression);

    /**
     * Get the progression configuration
     */
    static const ProgressionConfig& getProgressionConfig(ProgressionType progression);

  private:
    // Note names
    static const char *noteNames[12];

    // Static progression configurations
    static const ProgressionConfig progressionConfigs[22];

    int getScaleNote(int key, int degree, ProgressionType progression);
    juce::Array<int> getTriadIntervals(int quality);
    juce::Array<int> getSeventhIntervals(int quality);
    juce::Array<int> getNinthIntervals(int quality);
    juce::Array<int> getQuartalIntervals();
    juce::Array<int> getQuintalIntervals();
    juce::Array<int> applyVoicing(const juce::Array<int> &notes,
                                   ChordVoicing voicing);
};

} // namespace internal_plugins
