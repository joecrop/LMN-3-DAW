#include "Visualizer.h"

Visualizer::Visualizer() {
    orbs.reserve(12);
}

void Visualizer::paint(juce::Graphics &g) {
    if (orbs.empty())
        return;

    // Draw bloom effect first (background layer)
    drawBloom(g);

    // Draw connecting lines between orbs
    drawConnections(g);

    // Draw the orbs themselves
    drawOrbs(g);
}

void Visualizer::resized() {
    auto bounds = getLocalBounds().toFloat();
    center = bounds.getCentre();
    maxRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.35f;
}

void Visualizer::setEncoderColors(juce::Colour c1, juce::Colour c2,
                                  juce::Colour c3, juce::Colour c4) {
    encoderColors[0] = c1;
    encoderColors[1] = c2;
    encoderColors[2] = c3;
    encoderColors[3] = c4;
}

void Visualizer::update() {
    animTime += 0.016f;

    // Decay bloom
    if (bloomIntensity > 0) {
        bloomIntensity -= 0.02f;
        if (bloomIntensity < 0)
            bloomIntensity = 0;
    }

    updateOrbPositions();
}

void Visualizer::setChordNotes(const std::vector<int> &notes) {
    currentNotes = notes;

    // Create or update orbs for each note
    orbs.clear();

    if (notes.empty())
        return;

    int numNotes = static_cast<int>(notes.size());
    float angleStep = juce::MathConstants<float>::twoPi / numNotes;

    for (int i = 0; i < numNotes; ++i) {
        NoteOrb orb;
        orb.midiNote = notes[i];
        orb.targetAngle = i * angleStep;
        orb.angle = orb.targetAngle;

        // Radius based on octave (higher notes further out)
        int octave = notes[i] / 12;
        orb.radius = 0.4f + (octave - 4) * 0.15f;
        orb.radius = juce::jlimit(0.3f, 0.9f, orb.radius);

        // Size based on note's position in chord (root bigger)
        orb.size = (i == 0) ? 12.0f : 8.0f;

        // Color based on note
        orb.color = getColorForNote(notes[i]);
        orb.brightness = 1.0f;

        orb.trail.clear();

        orbs.push_back(orb);
    }
}

void Visualizer::triggerBloom(float intensity) {
    bloomIntensity = juce::jlimit(0.0f, 1.0f, intensity);

    // Increase orb brightness
    for (auto &orb : orbs) {
        orb.brightness = 1.0f + intensity * 0.5f;
    }
}

void Visualizer::updateOrbPositions() {
    for (auto &orb : orbs) {
        // Slowly rotate
        orb.angle += rotationSpeed;
        if (orb.angle > juce::MathConstants<float>::twoPi)
            orb.angle -= juce::MathConstants<float>::twoPi;

        // Calculate screen position
        float x = center.x + std::cos(orb.angle) * orb.radius * maxRadius;
        float y = center.y + std::sin(orb.angle) * orb.radius * maxRadius;

        // Add to trail
        orb.trail.insert(orb.trail.begin(), {x, y});
        if (orb.trail.size() > maxTrailLength)
            orb.trail.resize(maxTrailLength);

        // Decay brightness
        if (orb.brightness > 1.0f) {
            orb.brightness -= 0.02f;
            if (orb.brightness < 1.0f)
                orb.brightness = 1.0f;
        }
    }
}

void Visualizer::drawConnections(juce::Graphics &g) {
    if (orbs.size() < 2)
        return;

    // Draw lines connecting all orbs (constellation style)
    for (size_t i = 0; i < orbs.size(); ++i) {
        for (size_t j = i + 1; j < orbs.size(); ++j) {
            const auto &orb1 = orbs[i];
            const auto &orb2 = orbs[j];

            if (orb1.trail.empty() || orb2.trail.empty())
                continue;

            auto p1 = orb1.trail[0];
            auto p2 = orb2.trail[0];

            // Calculate distance for opacity
            float dist = p1.getDistanceFrom(p2);
            float maxDist = maxRadius * 2.0f;
            float alpha = 0.3f * (1.0f - dist / maxDist) + bloomIntensity * 0.2f;

            // Gradient line between the two orb colors
            juce::ColourGradient gradient(orb1.color.withAlpha(alpha), p1.x, p1.y,
                                          orb2.color.withAlpha(alpha), p2.x, p2.y,
                                          false);
            g.setGradientFill(gradient);
            g.drawLine(p1.x, p1.y, p2.x, p2.y, 1.0f + bloomIntensity);
        }
    }
}

void Visualizer::drawOrbs(juce::Graphics &g) {
    for (const auto &orb : orbs) {
        if (orb.trail.empty())
            continue;

        // Draw trail (motion blur)
        for (size_t i = 1; i < orb.trail.size(); ++i) {
            float trailAlpha = 0.3f * (1.0f - static_cast<float>(i) / orb.trail.size());
            float trailSize = orb.size * (1.0f - static_cast<float>(i) / orb.trail.size() * 0.5f);

            g.setColour(orb.color.withAlpha(trailAlpha));
            g.fillEllipse(orb.trail[i].x - trailSize / 2,
                          orb.trail[i].y - trailSize / 2, trailSize, trailSize);
        }

        // Draw glow
        float glowSize = orb.size * 2.5f * orb.brightness;
        g.setColour(orb.color.withAlpha(0.2f * orb.brightness));
        g.fillEllipse(orb.trail[0].x - glowSize / 2, orb.trail[0].y - glowSize / 2,
                      glowSize, glowSize);

        // Draw orb core
        float coreSize = orb.size * orb.brightness;
        g.setColour(orb.color.withAlpha(0.8f));
        g.fillEllipse(orb.trail[0].x - coreSize / 2, orb.trail[0].y - coreSize / 2,
                      coreSize, coreSize);

        // Draw bright center
        float centerSize = orb.size * 0.4f;
        g.setColour(juce::Colours::white.withAlpha(0.6f * orb.brightness));
        g.fillEllipse(orb.trail[0].x - centerSize / 2, orb.trail[0].y - centerSize / 2,
                      centerSize, centerSize);
    }
}

void Visualizer::drawBloom(juce::Graphics &g) {
    if (bloomIntensity < 0.05f)
        return;

    // Draw radial gradient bloom from center
    float bloomRadius = maxRadius * (1.0f + bloomIntensity);

    juce::ColourGradient radialGlow(
        juce::Colours::white.withAlpha(0.1f * bloomIntensity), center.x, center.y,
        juce::Colours::transparentWhite, center.x + bloomRadius, center.y, true);

    g.setGradientFill(radialGlow);
    g.fillEllipse(center.x - bloomRadius, center.y - bloomRadius,
                  bloomRadius * 2, bloomRadius * 2);
}

juce::Colour Visualizer::getColorForNote(int midiNote) {
    // Map note to one of the four encoder colors based on scale degree
    // This creates a consistent color mapping for chord tones

    int noteInOctave = midiNote % 12;

    // Root notes (C, F#) get blue
    if (noteInOctave == 0 || noteInOctave == 6)
        return encoderColors[0];

    // Third/seventh (E, B, Eb, Bb) get green
    if (noteInOctave == 4 || noteInOctave == 11 || noteInOctave == 3 || noteInOctave == 10)
        return encoderColors[1];

    // Fifth (G, D) get white
    if (noteInOctave == 7 || noteInOctave == 2)
        return encoderColors[2];

    // Other notes (9th, 11th, etc.) get red
    return encoderColors[3];
}
