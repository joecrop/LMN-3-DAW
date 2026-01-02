#include "InputListView.h"
#include <app_navigation/app_navigation.h>

InputListView::InputListView(tracktion::Edit &e, juce::AudioDeviceManager &dm,
                             app_services::MidiCommandManager &mcm)
    : deviceManager(dm), midiCommandManager(mcm), viewModel(e, deviceManager),
      titledList(viewModel.getItemNames(), "Audio Input",
                 ListTitle::IconType::FONT_AWESOME,
                 juce::String::charToString(0xf130)) {
    viewModel.itemListState.addListener(this);
    midiCommandManager.addListener(this);

    addAndMakeVisible(titledList);
}

InputListView::~InputListView() {
    midiCommandManager.removeListener(this);
    viewModel.itemListState.removeListener(this);
}

void InputListView::paint(juce::Graphics &g) {
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void InputListView::resized() {
    titledList.setBounds(getLocalBounds());
    titledList.getListView().getListBox().scrollToEnsureRowIsOnscreen(
        viewModel.itemListState.getSelectedItemIndex());
}

void InputListView::encoder1Increased() {
    if (isShowing()) {
        if (midiCommandManager.getFocusedComponent() == this) {
            viewModel.itemListState.setSelectedItemIndex(
                viewModel.itemListState.getSelectedItemIndex() + 1);
        }
    }
}

void InputListView::encoder1Decreased() {
    if (isShowing()) {
        if (midiCommandManager.getFocusedComponent() == this) {
            viewModel.itemListState.setSelectedItemIndex(
                viewModel.itemListState.getSelectedItemIndex() - 1);
        }
    }
}

void InputListView::encoder1ButtonReleased() {
    if (isShowing()) {
        if (midiCommandManager.getFocusedComponent() == this) {
            if (auto stackNavigationController = findParentComponentOfClass<
                    app_navigation::StackNavigationController>()) {
                stackNavigationController->popToRoot();
                midiCommandManager.setFocusedComponent(
                    stackNavigationController->getTopComponent());
            }
        }
    }
}

void InputListView::selectedIndexChanged(int newIndex) {
    titledList.getListView().getListBox().selectRow(newIndex);
    sendLookAndFeelChange();
}
