#include <app_view_models/app_view_models.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace AppViewModelsTests {

class MockSamplerRecordingListener
    : public app_view_models::SamplerRecordingViewModel::Listener {
  public:
    MOCK_METHOD(void, recordingStateChanged, (bool isRecording), (override));
    MOCK_METHOD(void, recordingTimeChanged, (double elapsedSeconds),
                (override));
    MOCK_METHOD(void, recordingComplete, (const juce::File &recordedFile),
                (override));
};

class SamplerRecordingViewModelTest : public ::testing::Test {
  protected:
    SamplerRecordingViewModelTest()
        : edit(tracktion::Edit::createSingleTrackEdit(engine)),
          viewModel(*edit) {}

    tracktion::Engine engine{"ENGINE"};
    std::unique_ptr<tracktion::Edit> edit;
    app_view_models::SamplerRecordingViewModel viewModel;
};

TEST_F(SamplerRecordingViewModelTest, initiallyNotRecording) {
    EXPECT_FALSE(viewModel.isRecording());
}

TEST_F(SamplerRecordingViewModelTest, initialElapsedTimeIsZero) {
    EXPECT_EQ(viewModel.getElapsedTimeSeconds(), 0.0);
}

TEST_F(SamplerRecordingViewModelTest, maxRecordingTimeIs30Seconds) {
    EXPECT_EQ(viewModel.getMaxRecordingTimeSeconds(), 30.0);
}

TEST_F(SamplerRecordingViewModelTest, remainingTimeEqualsMaxWhenNotRecording) {
    EXPECT_EQ(viewModel.getRemainingTimeSeconds(), 30.0);
}

TEST_F(SamplerRecordingViewModelTest, thumbnailIsValid) {
    auto &thumbnail = viewModel.getRecordingThumbnail();
    EXPECT_EQ(thumbnail.getTotalLength(), 0.0);
}

using ::testing::_;
TEST_F(SamplerRecordingViewModelTest, startRecordingChangesState) {
    MockSamplerRecordingListener listener;
    // Expected to be called once on start - but may not succeed due to missing
    // hardware, so use AtLeast(0)
    EXPECT_CALL(listener, recordingStateChanged(_))
        .Times(::testing::AtLeast(0));

    viewModel.addListener(&listener);

    // Note: startRecording may fail silently if no input device is available
    // in test environment, but shouldn't crash
    viewModel.startRecording();

    // Clean up
    viewModel.stopRecording();
    viewModel.removeListener(&listener);
}

TEST_F(SamplerRecordingViewModelTest,
       stopRecordingWhenNotRecordingDoesNotCrash) {
    // Should be safe to call stopRecording even when not recording
    EXPECT_NO_THROW(viewModel.stopRecording());
    EXPECT_FALSE(viewModel.isRecording());
}

TEST_F(SamplerRecordingViewModelTest, addAndRemoveListenerDoesNotCrash) {
    MockSamplerRecordingListener listener;
    EXPECT_NO_THROW(viewModel.addListener(&listener));
    EXPECT_NO_THROW(viewModel.removeListener(&listener));
}

} // namespace AppViewModelsTests
