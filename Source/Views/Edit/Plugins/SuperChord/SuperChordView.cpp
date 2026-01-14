#include "SuperChordView.h"
#include "SuperChordArpeggiatorPage.h"
#include "SuperChordModePage.h"
#include "SuperChordSoundPage.h"
#include "SuperChordVisualizerPage.h"

SuperChordView::SuperChordView(internal_plugins::SuperChordPlugin *p,
                               app_services::MidiCommandManager &mcm)
    : TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtTop),
      plugin(p), midiCommandManager(mcm) {
    addTab(visualizerTabName, juce::Colours::transparentBlack,
           new SuperChordVisualizerPage(plugin, midiCommandManager), true);
    addTab(soundTabName, juce::Colours::transparentBlack,
           new SuperChordSoundPage(plugin, midiCommandManager), true);
    addTab(modeTabName, juce::Colours::transparentBlack,
           new SuperChordModePage(plugin, midiCommandManager), true);
    addTab(arpeggiatorTabName, juce::Colours::transparentBlack,
           new SuperChordArpeggiatorPage(plugin, midiCommandManager), true);

    midiCommandManager.addListener(this);

    // Hide tab bar
    setTabBarDepth(0);

    // Start on visualizer page
    setCurrentTabIndex(0);

    pageLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                 getHeight() * .2, juce::Font::plain));
    pageLabel.setText(juce::String(getCurrentTabIndex() + 1) + "/" +
                          juce::String(getNumTabs()),
                      juce::dontSendNotification);
    pageLabel.setJustificationType(juce::Justification::centred);
    pageLabel.setColour(juce::Label::textColourId, appLookAndFeel.whiteColour);
    pageLabel.setAlwaysOnTop(true);
    addAndMakeVisible(pageLabel);
}

SuperChordView::~SuperChordView() { midiCommandManager.removeListener(this); }

void SuperChordView::paint(juce::Graphics &g) {
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SuperChordView::resized() {
    juce::TabbedComponent::resized();

    pageLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                 getHeight() * .05, juce::Font::plain));
    pageLabel.setBounds(
        getHeight() * .05, getHeight() * .05,
        pageLabel.getFont().getStringWidth(pageLabel.getText()) + 10,
        getHeight() * .05);
}

void SuperChordView::plusButtonReleased() {
    if (isShowing()) {
        int nextIndex = (getCurrentTabIndex() + 1) % getNumTabs();
        setCurrentTabIndex(nextIndex);
        midiCommandManager.setFocusedComponent(getCurrentContentComponent());
        pageLabel.setText(juce::String(getCurrentTabIndex() + 1) + "/" +
                              juce::String(getNumTabs()),
                          juce::dontSendNotification);
    }
}

void SuperChordView::minusButtonReleased() {
    if (isShowing()) {
        int prevIndex = (getCurrentTabIndex() - 1 + getNumTabs()) % getNumTabs();
        setCurrentTabIndex(prevIndex);
        midiCommandManager.setFocusedComponent(getCurrentContentComponent());
        pageLabel.setText(juce::String(getCurrentTabIndex() + 1) + "/" +
                              juce::String(getNumTabs()),
                          juce::dontSendNotification);
    }
}

void SuperChordView::visibilityChanged() {
    if (isShowing()) {
        midiCommandManager.setFocusedComponent(getCurrentContentComponent());
    }
}

void SuperChordView::parentHierarchyChanged() {
    visibilityChanged();
}
