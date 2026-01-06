#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <random>
#include <vector>

/**
 * ParticleSystem - Abstract visualization component for SuperChord
 *
 * Creates a cloud of floating particles that respond to:
 * - Encoder movements (pulses of colored particles)
 * - Note triggers (bursts of particles)
 * - Macro parameter values (particle behavior/appearance)
 */
class ParticleSystem : public juce::Component, private juce::Timer {
public:
    ParticleSystem();
    ~ParticleSystem() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    // Set encoder colors
    void setEncoderColors(juce::Colour c1, juce::Colour c2, juce::Colour c3,
                          juce::Colour c4);

    // Macro parameter controls
    void setWarmth(float value);  // Affects particle warmth/color temperature
    void setSpace(float value);   // Affects particle spread and drift
    void setAttack(float value);  // Affects particle sharpness and speed
    void setBloom(float value);   // Affects particle glow and size

    // Animation triggers
    void triggerEncoderPulse(int encoderIndex);  // Pulse specific color
    void setEncoderActivity(int encoderIndex, float activity);  // 0-1 activity level
    void triggerNoteBurst(float velocity);  // Burst on note trigger

private:
    void timerCallback() override;
    void spawnParticle(juce::Colour color, float intensity);
    void updateParticles();

    struct Particle {
        float x, y;           // Position (0-1 normalized)
        float vx, vy;         // Velocity
        float size;           // Size in pixels
        float alpha;          // Opacity
        float life;           // Remaining life (0-1)
        float decay;          // Life decay rate
        juce::Colour color;
        float glow;           // Glow intensity
    };

    std::vector<Particle> particles;
    static constexpr int maxParticles = 150;

    // Encoder colors
    juce::Colour encoderColors[4] = {
        juce::Colour(0xFF458588),  // Blue
        juce::Colour(0xFF689d6a),  // Green
        juce::Colour(0xFFf9f5d7),  // White
        juce::Colour(0xFFcc241d)   // Red
    };

    // Current macro values
    float warmth = 0.5f;
    float space = 0.5f;
    float attack = 0.5f;
    float bloom = 0.5f;

    // Encoder activity (for visual feedback)
    float encoderActivity[4] = {0, 0, 0, 0};

    // Random generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist{0.0f, 1.0f};

    // Ambient particle spawn timer
    float ambientSpawnTimer = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParticleSystem)
};
