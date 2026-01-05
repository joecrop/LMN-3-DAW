#pragma once

namespace app_view_models {

class SamplerRecordingViewModel : public juce::AudioIODeviceCallback,
                                  public juce::Timer {
  public:
    SamplerRecordingViewModel(tracktion::Edit &edit);
    ~SamplerRecordingViewModel() override;

    // Recording control
    void prepareNewRecording(); // Creates file and enters ready state
    void startRecording();      // Actually starts recording
    void stopRecording();
    bool isRecording() const;
    bool isReadyToRecord() const;
    void cancelRecording(); // Cancel without saving

    // Recording info
    double getElapsedTimeSeconds() const;
    double getMaxRecordingTimeSeconds() const;
    double getRemainingTimeSeconds() const;

    // Get the thumbnail for live waveform display
    juce::AudioThumbnail &getRecordingThumbnail();

    // AudioIODeviceCallback - receives audio from input device
    void audioDeviceIOCallback(const float **inputChannelData,
                               int numInputChannels, float **outputChannelData,
                               int numOutputChannels, int numSamples) override;
    void audioDeviceAboutToStart(juce::AudioIODevice *device) override;
    void audioDeviceStopped() override;

    // Timer callback for auto-stop and UI updates
    void timerCallback() override;

    class Listener {
      public:
        virtual ~Listener() = default;
        virtual void readyToRecordStateChanged(bool isReady) {}
        virtual void recordingStateChanged(bool isRecording) {}
        virtual void recordingTimeChanged(double elapsedSeconds) {}
        virtual void recordingComplete(const juce::File &recordedFile) {}
    };

    void addListener(Listener *l);
    void removeListener(Listener *l);

    static constexpr double MAX_RECORDING_TIME_SECONDS = 30.0;

  private:
    tracktion::Edit &edit;
    tracktion::Engine &engine;

    std::atomic<bool> recording{false};
    std::atomic<bool> readyToRecord{false};
    std::atomic<double> elapsedTimeSeconds{0.0};
    double sampleRate = 44100.0;
    int samplesRecorded = 0;

    // Audio file writing
    std::unique_ptr<tracktion::AudioFileWriter> audioFileWriter;
    juce::File currentRecordingFile;

    // Thumbnail for live display
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{1};
    juce::AudioThumbnail recordingThumbnail;

    // Temporary buffer for converting to juce buffer
    juce::AudioBuffer<float> tempBuffer;

    juce::ListenerList<Listener> listeners;

    juce::File generateRecordingFilename();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplerRecordingViewModel)
};

} // namespace app_view_models
