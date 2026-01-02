#pragma once

namespace app_view_models {
namespace IDs {
const juce::Identifier INPUT_LIST_VIEW_STATE("INPUT_LIST_VIEW_STATE");
}

class InputListViewModel : private ItemListState::Listener {
  public:
    InputListViewModel(tracktion::Edit &e, juce::AudioDeviceManager &dm);

    ~InputListViewModel() override;
    juce::StringArray getItemNames();
    juce::String getSelectedItem();
    void updateInput();

  private:
    juce::AudioDeviceManager &deviceManager;
    juce::ValueTree state;
    juce::StringArray inputs;

    void selectedIndexChanged(int newIndex) override;

  public:
    // Must appear below the other variables since it needs to be initialized
    // last
    ItemListState itemListState;
};

} // namespace app_view_models
