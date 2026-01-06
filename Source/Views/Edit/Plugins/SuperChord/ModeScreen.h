#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

/**
 * ModeScreen - Abstract mode/preset selection overlay
 *
 * Displayed when encoder 1 is pressed.
 * Uses abstract visuals (pulsing orbs, no text) to select:
 * - Scale type
 * - Play mode (Chord/Arp/Strum)
 * - Voice preset
 */
class ModeScreen : public juce::Component, private juce::Timer {
public:
    ModeScreen();
    ~ModeScreen() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    // Navigation
    void navigateNext();
    void navigatePrevious();
    void selectCurrent();
    void goBack();
    void reset();

    // State queries
    bool isAtTopLevel() const { return currentLevel == 0; }
    int getSelectedCategory() const { return selectedCategory; }
    int getSelectedItem() const { return selectedItem; }

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
    void drawCategoryOrbs(juce::Graphics &g);
    void drawItemOrbs(juce::Graphics &g);
    juce::Colour getCategoryColor(int category);

    // Navigation state
    int currentLevel = 0;  // 0 = categories, 1 = items within category
    int selectedCategory = 0;
    int selectedItem = 0;

    // Categories: 0=Scale, 1=PlayMode, 2=VoicePreset
    static constexpr int numCategories = 3;
    static constexpr int numScales = 7;
    static constexpr int numPlayModes = 3;
    static constexpr int numVoicePresets = 8;

    // Animation
    float animTime = 0.0f;
    float selectionPulse = 0.0f;

    // Encoder colors for visual mapping
    juce::Colour scaleColor{0xFF458588};      // Blue
    juce::Colour playModeColor{0xFF689d6a};   // Green
    juce::Colour voicePresetColor{0xFFcc241d}; // Red

    std::vector<Listener *> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeScreen)
};
