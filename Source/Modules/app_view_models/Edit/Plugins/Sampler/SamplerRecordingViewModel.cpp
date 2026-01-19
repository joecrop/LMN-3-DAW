namespace app_view_models {

SamplerRecordingViewModel::SamplerRecordingViewModel(tracktion::Edit &e)
    : edit(e), engine(e.engine), formatManager(),
      recordingThumbnail(512, formatManager, thumbnailCache) {
    formatManager.registerBasicFormats();

    // Get sample rate from device manager
    auto *currentDevice =
        engine.getDeviceManager().deviceManager.getCurrentAudioDevice();
    if (currentDevice != nullptr) {
        sampleRate = currentDevice->getCurrentSampleRate();
    }
}

SamplerRecordingViewModel::~SamplerRecordingViewModel() {
    stopTimer();
    if (recording.load()) {
        engine.getDeviceManager().deviceManager.removeAudioCallback(this);
    }
}

void SamplerRecordingViewModel::prepareNewRecording() {
    if (recording.load() || readyToRecord.load()) {
        return;
    }

    // Generate filename with timestamp
    currentRecordingFile = generateRecordingFilename();

    // Create parent directory if needed
    currentRecordingFile.getParentDirectory().createDirectory();

    // Get sample rate
    auto *currentDevice =
        engine.getDeviceManager().deviceManager.getCurrentAudioDevice();
    if (currentDevice != nullptr) {
        sampleRate = currentDevice->getCurrentSampleRate();
    }

    // Create audio file for writing (mono, 16-bit WAV)
    tracktion::AudioFile audioFile(engine, currentRecordingFile);
    auto *wavFormat = engine.getAudioFileFormatManager().getWavFormat();

    audioFileWriter = std::make_unique<tracktion::AudioFileWriter>(
        audioFile, wavFormat, 1, // mono
        sampleRate, 16,          // 16-bit
        juce::StringPairArray(), 0);

    if (!audioFileWriter->isOpen()) {
        juce::Logger::writeToLog("Failed to open audio file for recording: " +
                                 currentRecordingFile.getFullPathName());
        audioFileWriter.reset();
        return;
    }

    // Reset state
    samplesRecorded = 0;
    elapsedTimeSeconds.store(0.0);
    recordingThumbnail.reset(1, sampleRate);

    readyToRecord.store(true);
    listeners.call(&Listener::readyToRecordStateChanged, true);
}

void SamplerRecordingViewModel::startRecording() {
    if (recording.load()) {
        return;
    }

    // If not ready, prepare first
    if (!readyToRecord.load()) {
        prepareNewRecording();
        if (!readyToRecord.load()) {
            return; // Failed to prepare
        }
    }

    // Register as audio callback to receive input
    engine.getDeviceManager().deviceManager.addAudioCallback(this);

    recording.store(true);

    // Start timer for updates and auto-stop (100ms interval)
    startTimer(100);

    listeners.call(&Listener::recordingStateChanged, true);
}

void SamplerRecordingViewModel::stopRecording() {
    if (!recording.load()) {
        return;
    }

    stopTimer();
    recording.store(false);
    readyToRecord.store(false);

    // Remove audio callback
    engine.getDeviceManager().deviceManager.removeAudioCallback(this);

    // Close the audio file
    if (audioFileWriter) {
        audioFileWriter->closeForWriting();
        audioFileWriter.reset();
    }

    listeners.call(&Listener::recordingStateChanged, false);
    listeners.call(&Listener::readyToRecordStateChanged, false);
    listeners.call(&Listener::recordingComplete, currentRecordingFile);
}

void SamplerRecordingViewModel::cancelRecording() {
    stopTimer();

    if (recording.load()) {
        engine.getDeviceManager().deviceManager.removeAudioCallback(this);
    }

    recording.store(false);
    readyToRecord.store(false);

    // Close and delete the audio file
    if (audioFileWriter) {
        audioFileWriter->closeForWriting();
        audioFileWriter.reset();
    }

    if (currentRecordingFile.existsAsFile()) {
        currentRecordingFile.deleteFile();
    }

    listeners.call(&Listener::recordingStateChanged, false);
    listeners.call(&Listener::readyToRecordStateChanged, false);
}

bool SamplerRecordingViewModel::isRecording() const { return recording.load(); }

bool SamplerRecordingViewModel::isReadyToRecord() const {
    return readyToRecord.load();
}

double SamplerRecordingViewModel::getElapsedTimeSeconds() const {
    return elapsedTimeSeconds.load();
}

double SamplerRecordingViewModel::getMaxRecordingTimeSeconds() const {
    return MAX_RECORDING_TIME_SECONDS;
}

double SamplerRecordingViewModel::getRemainingTimeSeconds() const {
    return MAX_RECORDING_TIME_SECONDS - elapsedTimeSeconds.load();
}

juce::AudioThumbnail &SamplerRecordingViewModel::getRecordingThumbnail() {
    return recordingThumbnail;
}

void SamplerRecordingViewModel::audioDeviceIOCallback(
    const float **inputChannelData, int numInputChannels,
    float **outputChannelData, int numOutputChannels, int numSamples) {
    // Clear output to avoid feedback
    for (int i = 0; i < numOutputChannels; ++i) {
        if (outputChannelData[i] != nullptr) {
            juce::FloatVectorOperations::clear(outputChannelData[i],
                                               numSamples);
        }
    }

    if (!recording.load() || !audioFileWriter || !audioFileWriter->isOpen()) {
        return;
    }

    if (numInputChannels <= 0 || inputChannelData == nullptr) {
        return;
    }

    // Resize temp buffer if needed
    if (tempBuffer.getNumSamples() < numSamples) {
        tempBuffer.setSize(1, numSamples, false, false, true);
    }

    // Convert to mono by taking first channel or averaging
    tempBuffer.clear();
    auto *dest = tempBuffer.getWritePointer(0);

    if (inputChannelData[0] != nullptr) {
        juce::FloatVectorOperations::copy(dest, inputChannelData[0],
                                          numSamples);

        // If stereo, average the channels
        if (numInputChannels > 1 && inputChannelData[1] != nullptr) {
            juce::FloatVectorOperations::add(dest, inputChannelData[1],
                                             numSamples);
            juce::FloatVectorOperations::multiply(dest, 0.5f, numSamples);
        }
    }

    // Write to file
    audioFileWriter->appendBuffer(tempBuffer, numSamples);

    // Update thumbnail for live display
    recordingThumbnail.addBlock(samplesRecorded, tempBuffer, 0, numSamples);

    // Update sample count and elapsed time
    samplesRecorded += numSamples;
    elapsedTimeSeconds.store(static_cast<double>(samplesRecorded) / sampleRate);
}

void SamplerRecordingViewModel::audioDeviceAboutToStart(
    juce::AudioIODevice *device) {
    if (device != nullptr) {
        sampleRate = device->getCurrentSampleRate();
    }
}

void SamplerRecordingViewModel::audioDeviceStopped() {
    // Nothing needed here
}

void SamplerRecordingViewModel::timerCallback() {
    // Notify listeners of time change
    listeners.call(&Listener::recordingTimeChanged, elapsedTimeSeconds.load());

    // Auto-stop if max time reached
    if (elapsedTimeSeconds.load() >= MAX_RECORDING_TIME_SECONDS) {
        stopRecording();
    }
}

void SamplerRecordingViewModel::addListener(Listener *l) { listeners.add(l); }

void SamplerRecordingViewModel::removeListener(Listener *l) {
    listeners.remove(l);
}

juce::File SamplerRecordingViewModel::generateRecordingFilename() {
    auto now = juce::Time::getCurrentTime();
    // Format: rec_jan_01_2025_1:30:45PM
    auto month = now.getMonthName(true).toLowerCase();
    auto day = juce::String(now.getDayOfMonth());
    auto year = juce::String(now.getYear());
    auto hour = now.getHours();
    auto minute = now.getMinutes();
    auto second = now.getSeconds();
    auto ampm = hour >= 12 ? "PM" : "AM";
    auto hour12 = hour % 12;
    if (hour12 == 0)
        hour12 = 12;

    auto timestamp = juce::String::formatted(
        "%s_%s_%s_%d:%02d:%02d%s", month.toRawUTF8(), day.toRawUTF8(),
        year.toRawUTF8(), hour12, minute, second, ampm);
    auto filename = "rec_" + timestamp + ".wav";

    return ConfigurationHelpers::getTempRecordedSamplesDirectory(engine)
        .getChildFile(filename);
}

} // namespace app_view_models
