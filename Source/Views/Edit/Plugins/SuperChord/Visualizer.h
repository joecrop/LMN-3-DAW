#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

/**
 * Visualizer - Chord constellation and motion trail visualization
 *
 * Displays:
 * - Chord notes as orbiting "orbs" in a constellation pattern
 * - Motion trails behind moving elements
 * - Bloom effects on note triggers
 */
class Visualizer : public juce::Component {
public:
    Visualizer();
    ~Visualizer() override = default;

    void paint(juce::Graphics &g) override;
    void resized() override;

    // Set encoder colors
    void setEncoderColors(juce::Colour c1, juce::Colour c2, juce::Colour c3,
                          juce::Colour c4);

    // Update animation state
    void update();

    // Set current chord notes (MIDI note numbers)
    void setChordNotes(const std::vector<int> &notes);

    // Trigger bloom effect
    void triggerBloom(float intensity);

private:
    struct NoteOrb {
        int midiNote;
        float angle;       // Position angle around center
        float targetAngle;
        float radius;      // Distance from center
        float size;        // Orb size
        juce::Colour color;
        float brightness;
        
        // Trail positions (for motion blur)
        std::vector<juce::Point<float>> trail;
    };

    void updateOrbPositions();
    void drawConnections(juce::Graphics &g);
    void drawOrbs(juce::Graphics &g);
    void drawBloom(juce::Graphics &g);
    juce::Colour getColorForNote(int midiNote);

    std::vector<NoteOrb> orbs;
    std::vector<int> currentNotes;

    // Encoder colors
    juce::Colour encoderColors[4] = {
        juce::Colour(0xFF458588),  // Blue
        juce::Colour(0xFF689d6a),  // Green
        juce::Colour(0xFFf9f5d7),  // White
        juce::Colour(0xFFcc241d)   // Red
    };

    // Animation state
    float animTime = 0.0f;
    float bloomIntensity = 0.0f;
    float rotationSpeed = 0.002f;

    // Geometry
    juce::Point<float> center;
    float maxRadius = 100.0f;

    static constexpr int maxTrailLength = 8;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Visualizer)
};
