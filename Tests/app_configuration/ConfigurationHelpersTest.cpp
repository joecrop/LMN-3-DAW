#include <app_configuration/app_configuration.h>
#include <gtest/gtest.h>

namespace AppConfigurationTests {

class ConfigurationHelpersTest : public ::testing::Test {
  protected:
    tracktion::Engine engine{"ENGINE"};
};

TEST_F(ConfigurationHelpersTest, recordedSamplesDirectoryIsUnderSamplesDir) {
    auto samplesDir = ConfigurationHelpers::getSamplesDirectory();
    auto recordedSamplesDir =
        ConfigurationHelpers::getRecordedSamplesDirectory();

    EXPECT_EQ(recordedSamplesDir.getParentDirectory(), samplesDir);
}

TEST_F(ConfigurationHelpersTest,
       recordedSamplesDirectoryNameIsRecordedSamples) {
    auto recordedSamplesDir =
        ConfigurationHelpers::getRecordedSamplesDirectory();

    EXPECT_EQ(recordedSamplesDir.getFileName(), "recorded_samples");
}

TEST_F(ConfigurationHelpersTest,
       tempRecordedSamplesDirectoryIsUnderTempSamplesDir) {
    auto tempSamplesDir = ConfigurationHelpers::getTempSamplesDirectory(engine);
    auto tempRecordedSamplesDir =
        ConfigurationHelpers::getTempRecordedSamplesDirectory(engine);

    EXPECT_EQ(tempRecordedSamplesDir.getParentDirectory(), tempSamplesDir);
}

TEST_F(ConfigurationHelpersTest,
       tempRecordedSamplesDirectoryNameIsRecordedSamples) {
    auto tempRecordedSamplesDir =
        ConfigurationHelpers::getTempRecordedSamplesDirectory(engine);

    EXPECT_EQ(tempRecordedSamplesDir.getFileName(), "recorded_samples");
}

} // namespace AppConfigurationTests
