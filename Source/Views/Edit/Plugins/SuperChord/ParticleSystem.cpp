#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {
    std::random_device rd;
    rng.seed(rd());
    particles.reserve(maxParticles);
    startTimerHz(60);
}

ParticleSystem::~ParticleSystem() {
    stopTimer();
}

void ParticleSystem::paint(juce::Graphics &g) {
    auto bounds = getLocalBounds().toFloat();

    for (const auto &p : particles) {
        if (p.life <= 0)
            continue;

        float x = p.x * bounds.getWidth();
        float y = p.y * bounds.getHeight();
        float size = p.size;

        // Draw glow layer first (if bloom enabled)
        if (p.glow > 0.1f) {
            float glowSize = size * (2.0f + bloom * 2.0f);
            g.setColour(p.color.withAlpha(p.alpha * 0.3f * p.glow));
            g.fillEllipse(x - glowSize / 2, y - glowSize / 2, glowSize, glowSize);
        }

        // Draw particle core
        g.setColour(p.color.withAlpha(p.alpha));
        g.fillEllipse(x - size / 2, y - size / 2, size, size);

        // Draw bright center
        float coreSize = size * 0.4f;
        g.setColour(juce::Colours::white.withAlpha(p.alpha * 0.5f));
        g.fillEllipse(x - coreSize / 2, y - coreSize / 2, coreSize, coreSize);
    }
}

void ParticleSystem::resized() {
    // Nothing specific needed
}

void ParticleSystem::setEncoderColors(juce::Colour c1, juce::Colour c2,
                                      juce::Colour c3, juce::Colour c4) {
    encoderColors[0] = c1;
    encoderColors[1] = c2;
    encoderColors[2] = c3;
    encoderColors[3] = c4;
}

void ParticleSystem::setWarmth(float value) {
    warmth = juce::jlimit(0.0f, 1.0f, value);
}

void ParticleSystem::setSpace(float value) {
    space = juce::jlimit(0.0f, 1.0f, value);
}

void ParticleSystem::setAttack(float value) {
    attack = juce::jlimit(0.0f, 1.0f, value);
}

void ParticleSystem::setBloom(float value) {
    bloom = juce::jlimit(0.0f, 1.0f, value);
}

void ParticleSystem::triggerEncoderPulse(int encoderIndex) {
    if (encoderIndex < 0 || encoderIndex > 3)
        return;

    // Spawn a burst of particles in the encoder's color
    juce::Colour color = encoderColors[encoderIndex];

    // Position based on encoder (corners)
    float baseX, baseY;
    switch (encoderIndex) {
        case 0: baseX = 0.15f; baseY = 0.85f; break;  // Bottom left
        case 1: baseX = 0.38f; baseY = 0.85f; break;  // Bottom center-left
        case 2: baseX = 0.62f; baseY = 0.85f; break;  // Bottom center-right
        case 3: baseX = 0.85f; baseY = 0.85f; break;  // Bottom right
        default: baseX = 0.5f; baseY = 0.5f;
    }

    // Spawn 8-12 particles
    int count = 8 + static_cast<int>(dist(rng) * 4);
    for (int i = 0; i < count && particles.size() < maxParticles; ++i) {
        spawnParticle(color, 1.0f);
        if (!particles.empty()) {
            auto &p = particles.back();
            p.x = baseX + (dist(rng) - 0.5f) * 0.1f;
            p.y = baseY + (dist(rng) - 0.5f) * 0.1f;
            p.vy = -0.005f - dist(rng) * 0.01f;  // Rise upward
            p.vx = (dist(rng) - 0.5f) * 0.01f;
        }
    }
}

void ParticleSystem::setEncoderActivity(int encoderIndex, float activity) {
    if (encoderIndex >= 0 && encoderIndex < 4) {
        encoderActivity[encoderIndex] = juce::jlimit(0.0f, 1.0f, activity);
    }
}

void ParticleSystem::triggerNoteBurst(float velocity) {
    // Large central burst mixing all encoder colors
    int baseCount = static_cast<int>(10 + velocity * 20);

    for (int i = 0; i < baseCount && particles.size() < maxParticles; ++i) {
        // Choose a random encoder color, weighted by macro values
        float weights[4] = {warmth, space, attack, bloom};
        float totalWeight = warmth + space + attack + bloom + 0.1f;
        float r = dist(rng) * totalWeight;

        int colorIdx = 0;
        float cumulative = 0;
        for (int j = 0; j < 4; ++j) {
            cumulative += weights[j];
            if (r <= cumulative) {
                colorIdx = j;
                break;
            }
        }

        spawnParticle(encoderColors[colorIdx], velocity);
        if (!particles.empty()) {
            auto &p = particles.back();
            p.x = 0.5f + (dist(rng) - 0.5f) * 0.2f;
            p.y = 0.5f + (dist(rng) - 0.5f) * 0.2f;

            // Radial explosion
            float angle = dist(rng) * juce::MathConstants<float>::twoPi;
            float speed = 0.01f + dist(rng) * 0.02f * velocity;
            p.vx = std::cos(angle) * speed;
            p.vy = std::sin(angle) * speed;
            p.size *= (1.0f + velocity * 0.5f);
        }
    }
}

void ParticleSystem::timerCallback() {
    updateParticles();

    // Spawn ambient particles based on macro values
    ambientSpawnTimer += 0.016f;  // ~60fps
    float spawnRate = 0.1f + space * 0.2f;  // More particles with more space

    if (ambientSpawnTimer > spawnRate && particles.size() < maxParticles - 20) {
        ambientSpawnTimer = 0;

        // Pick a random encoder color
        int colorIdx = static_cast<int>(dist(rng) * 4) % 4;
        spawnParticle(encoderColors[colorIdx], 0.3f);

        if (!particles.empty()) {
            auto &p = particles.back();
            // Random position, weighted toward edges
            if (dist(rng) > 0.5f) {
                p.x = dist(rng) > 0.5f ? 0.0f : 1.0f;
                p.y = dist(rng);
            } else {
                p.x = dist(rng);
                p.y = dist(rng) > 0.5f ? 0.0f : 1.0f;
            }
            p.alpha = 0.3f;
            p.decay = 0.002f;
        }
    }

    repaint();
}

void ParticleSystem::spawnParticle(juce::Colour color, float intensity) {
    if (particles.size() >= maxParticles) {
        // Remove oldest dead particle
        auto it = std::find_if(particles.begin(), particles.end(),
                               [](const Particle &p) { return p.life <= 0; });
        if (it != particles.end()) {
            particles.erase(it);
        } else {
            return;  // No room
        }
    }

    Particle p;
    p.x = 0.5f;
    p.y = 0.5f;
    p.vx = 0;
    p.vy = 0;

    // Size affected by warmth (warmer = softer = larger)
    p.size = 3.0f + warmth * 5.0f + intensity * 3.0f;

    // Alpha affected by intensity
    p.alpha = 0.4f + intensity * 0.5f;

    // Life and decay affected by attack (more attack = shorter, sharper life)
    p.life = 1.0f;
    p.decay = 0.005f + attack * 0.015f;

    // Glow affected by bloom
    p.glow = bloom * intensity;

    // Apply warmth to color (shift toward warmer tones)
    float warmShift = warmth * 0.3f;
    p.color = color.withMultipliedSaturation(1.0f - warmShift * 0.2f)
                  .interpolatedWith(juce::Colours::orange, warmShift * 0.2f);

    particles.push_back(p);
}

void ParticleSystem::updateParticles() {
    float driftStrength = 0.001f + space * 0.003f;  // More space = more drift
    float speedMultiplier = 0.5f + attack * 1.0f;   // More attack = faster

    for (auto &p : particles) {
        if (p.life <= 0)
            continue;

        // Apply velocity
        p.x += p.vx * speedMultiplier;
        p.y += p.vy * speedMultiplier;

        // Add subtle drift toward center (or away with high space)
        float driftX = (0.5f - p.x) * driftStrength;
        float driftY = (0.5f - p.y) * driftStrength;

        if (space > 0.6f) {
            // High space = particles drift outward
            driftX *= -1;
            driftY *= -1;
        }

        p.vx += driftX;
        p.vy += driftY;

        // Damping
        p.vx *= 0.98f;
        p.vy *= 0.98f;

        // Decay life
        p.life -= p.decay;
        p.alpha = juce::jmax(0.0f, p.alpha - p.decay * 0.5f);

        // Wrap at edges
        if (p.x < 0) p.x = 1.0f;
        if (p.x > 1) p.x = 0.0f;
        if (p.y < 0) p.y = 1.0f;
        if (p.y > 1) p.y = 0.0f;
    }

    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle &p) { return p.life <= 0 && p.alpha <= 0; }),
        particles.end());
}
