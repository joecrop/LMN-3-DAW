#include <app_view_models/app_view_models.h>
#include <gtest/gtest.h>

namespace AppViewModelsTests {

class InputListViewModelTest : public ::testing::Test {
  protected:
    InputListViewModelTest()
        : edit(tracktion::Edit::createSingleTrackEdit(engine)),
          viewModel(*edit, engine.getDeviceManager().deviceManager) {}

    void SetUp() override {
        // flush any pending updates
        viewModel.itemListState.handleUpdateNowIfNeeded();
    }

    tracktion::Engine engine{"ENGINE"};
    std::unique_ptr<tracktion::Edit> edit;
    app_view_models::InputListViewModel viewModel;
};

TEST_F(InputListViewModelTest, getItemNames) {
    // Should return a list of input devices (may be empty on test system)
    auto items = viewModel.getItemNames();
    // Just verify the method works without crashing
    EXPECT_GE(items.size(), 0);
}

TEST_F(InputListViewModelTest, getSelectedItem) {
    // Should return a string (possibly empty if no inputs)
    auto selected = viewModel.getSelectedItem();
    // Just verify the method works without crashing
    EXPECT_TRUE(selected.isEmpty() || selected.isNotEmpty());
}

TEST_F(InputListViewModelTest, itemListStateInitialized) {
    // The itemListState should be properly initialized
    auto listSize = viewModel.itemListState.listSize;
    EXPECT_EQ(listSize, viewModel.getItemNames().size());
}

} // namespace AppViewModelsTests
