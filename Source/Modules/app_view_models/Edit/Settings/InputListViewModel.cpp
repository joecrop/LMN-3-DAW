namespace app_view_models {
InputListViewModel::InputListViewModel(tracktion::Edit &e,
                                       juce::AudioDeviceManager &dm)
    : deviceManager(dm),
      state(
          e.state.getOrCreateChildWithName(IDs::SETTINGS_VIEW_STATE, nullptr)
              .getOrCreateChildWithName(IDs::INPUT_LIST_VIEW_STATE, nullptr)),
      itemListState(state, inputs.size()) {
    // Get input device names (true = get input devices)
    for (const auto &deviceName :
         deviceManager.getCurrentDeviceTypeObject()->getDeviceNames(true)) {
        inputs.add(deviceName);
    }
    itemListState.listSize = inputs.size();

    auto currentDevice = deviceManager.getAudioDeviceSetup().inputDeviceName;
    int currentInputIndex = 0;
    if (currentDevice != "") {
        currentInputIndex = inputs.indexOf(currentDevice);
        if (currentInputIndex == -1) {
            currentInputIndex = 0;
        }
    } else {
        juce::Logger::writeToLog("current input device name is empty");
    }
    itemListState.setSelectedItemIndex(currentInputIndex);
    itemListState.addListener(this);
}

InputListViewModel::~InputListViewModel() { itemListState.removeListener(this); }

juce::StringArray InputListViewModel::getItemNames() { return inputs; }

juce::String InputListViewModel::getSelectedItem() {
    if (inputs.size() > 0 &&
        itemListState.getSelectedItemIndex() < inputs.size()) {
        return inputs[itemListState.getSelectedItemIndex()];
    }
    return "";
}

void InputListViewModel::updateInput() {
    auto setup = deviceManager.getAudioDeviceSetup();
    setup.inputDeviceName = getSelectedItem();
    auto result = deviceManager.setAudioDeviceSetup(setup, true);
    if (result != "") {
        juce::Logger::writeToLog("Error setting input device to " +
                                 getSelectedItem() + ": " + result);
    }
}

void InputListViewModel::selectedIndexChanged(int newIndex) { updateInput(); }
} // namespace app_view_models
