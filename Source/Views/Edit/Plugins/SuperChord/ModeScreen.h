#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

/**
 * ModeScreen - Mode/preset selection overlay with direct encoder control
 *
 * Displayed when encoder 1 is pressed.
 * Each encoder directly controls its setting:
 * - Blue (Encoder 1): Scale type
 * - Green (Encoder 2): Play mode (Chord/Arp/Strum)
 * - Red (Encoder 4): Voice preset
 */
class ModeScreen : public juce::Component, private juce::Timer {
public:
    ModeScreen();
    ~ModeScreen() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    // Direct encoder control - each encoder changes its own setting
    void nextScale();
    void previousScale();
    void nextPlayMode();
    void previousPlayMode();
    void nextVoicePreset();
    void previousVoicePreset();
    
    // Trigger visual pulse for encoder activity
    void triggerScalePulse() { scalePulse = 1.0f; }
    void triggerPlayModePulse() { playModePulse = 1.0f; }
    void triggerVoicePulse() { voicePulse = 1.0f; }

    // State queries
    int getCurrentScale() const { return currentScale; }
    int getCurrentPlayMode() const { return currentPlayMode; }
    int getCurrentVoicePreset() const { return currentVoicePreset; }
    
    // Set current values (for syncing with plugin state)
    void setCurrentScale(int scale) { currentScale = scale % numScales; }
    void setCurrentPlayMode(int mode) { currentPlayMode = mode % numPlayModes; }
    void setCurrentVoicePreset(int preset) { currentVoicePreset = preset % numVoicePresets; }

    // Listener for selection changes
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void scaleTypeChanged(int newType) = 0;
        virtual void playModeChanged(int newMode) = 0;
        virtual void voicePresetChanged(int newPreset) = 0;
    };

    void addListener(Listener *l) { listeners.push_back(l); }
    void removeListener(Listener *l) {
        listeners.erase(std::remove(listeners.begin(), listeners.end(), l),
                        listeners.end());
    }

private:
    void timerCallback() override;
    void drawSettingsPanel(juce::Graphics &g);
    juce::Colour getCategoryColor(int category);
    juce::String getCategoryName(int category);
    juce::String getItemName(int category, int item);

    // Current values for each setting
    int currentScale = 0;
    int currentPlayMode = 0;
    int currentVoicePreset = 0;

    // Settings counts
    static constexpr int numScales = 7;
    static constexpr int numPlayModes = 3;
    static constexpr int numVoicePresets = 8;

    // Animation
    float animTime = 0.0f;
    float scalePulse = 0.0f;
    float playModePulse = 0.0f;
    float voicePulse = 0.0f;

    // Encoder colors for visual mapping
    juce::Colour scaleColor{0xFF458588};      // Blue (Encoder 1)
    juce::Colour playModeColor{0xFF689d6a};   // Green (Encoder 2)
    juce::Colour voicePresetColor{0xFFcc241d}; // Red (Encoder 4)

    std::vector<Listener *> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeScreen)
};
