#include "SamplerView.h"
#include <internal_plugins/internal_plugins.h>

SamplerView::SamplerView(tracktion::SamplerPlugin *sampler,
                         app_services::MidiCommandManager &mcm,
                         tracktion::Edit &edit)
    : samplerPlugin(sampler), midiCommandManager(mcm),
      viewModel(std::unique_ptr<app_view_models::SamplerViewModel>(
          std::make_unique<app_view_models::SynthSamplerViewModel>(sampler))),
      recordingViewModel(
          std::make_unique<app_view_models::SamplerRecordingViewModel>(edit)),
      fullSampleThumbnail(viewModel->getFullSampleThumbnail(),
                          appLookAndFeel.colour1.withAlpha(.3f)),
      sampleExcerptThumbnail(viewModel->getFullSampleThumbnail(),
                             appLookAndFeel.colour1),
      titledList(viewModel->getItemNames(), viewModel->getTitle(),
                 ListTitle::IconType::FONT_AWESOME,
                 juce::String::charToString(0xf478)) {
    init();
}

SamplerView::SamplerView(internal_plugins::DrumSamplerPlugin *drumSampler,
                         app_services::MidiCommandManager &mcm,
                         tracktion::Edit &edit)
    : samplerPlugin(drumSampler), midiCommandManager(mcm),
      viewModel(std::unique_ptr<app_view_models::SamplerViewModel>(
          std::make_unique<app_view_models::DrumSamplerViewModel>(
              drumSampler))),
      recordingViewModel(
          std::make_unique<app_view_models::SamplerRecordingViewModel>(edit)),
      fullSampleThumbnail(viewModel->getFullSampleThumbnail(),
                          appLookAndFeel.colour1.withAlpha(.3f)),
      sampleExcerptThumbnail(viewModel->getFullSampleThumbnail(),
                             appLookAndFeel.colour1),
      titledList(viewModel->getItemNames(), viewModel->getTitle(),
                 ListTitle::IconType::FONT_AWESOME,
                 juce::String::charToString(0xf569)) {
    init();
}

void SamplerView::init() {
    addAndMakeVisible(fullSampleThumbnail);
    addAndMakeVisible(sampleExcerptThumbnail);

    sampleLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                   getHeight() * .1, juce::Font::plain));
    sampleLabel.setText(viewModel->getSelectedItemName(),
                        juce::dontSendNotification);
    sampleLabel.setJustificationType(juce::Justification::centred);
    sampleLabel.setColour(juce::Label::textColourId, appLookAndFeel.colour1);
    addAndMakeVisible(sampleLabel);

    gainLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                 getHeight() * .1, juce::Font::plain));
    gainLabel.setText(juce::String(viewModel->getGain()),
                      juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centredRight);
    gainLabel.setColour(juce::Label::textColourId, appLookAndFeel.colour4);
    addChildComponent(gainLabel);

    // Recording time label
    recordingTimeLabel.setFont(
        juce::Font(juce::Font::getDefaultMonospacedFontName(), getHeight() * .1,
                   juce::Font::plain));
    recordingTimeLabel.setJustificationType(juce::Justification::centred);
    recordingTimeLabel.setColour(juce::Label::textColourId,
                                 appLookAndFeel.colour3);
    addChildComponent(recordingTimeLabel);

    // Recording thumbnail
    recordingThumbnail = std::make_unique<ThumbnailComponent>(
        recordingViewModel->getRecordingThumbnail(), appLookAndFeel.colour3);
    addChildComponent(*recordingThumbnail);

    startMarker.setFill(juce::FillType(appLookAndFeel.colour2));
    endMarker.setFill(juce::FillType(appLookAndFeel.colour3));
    addAndMakeVisible(startMarker);
    addAndMakeVisible(endMarker);

    addChildComponent(titledList);

    if (viewModel->getItemNames().size() <= 0) {
        emptyLabel.setFont(
            juce::Font(juce::Font::getDefaultMonospacedFontName(),
                       getHeight() * .1, juce::Font::plain));
        emptyLabel.setJustificationType(juce::Justification::centred);
        emptyLabel.setAlwaysOnTop(true);
        emptyLabel.setColour(juce::Label::textColourId, appLookAndFeel.colour1);
        emptyLabel.setText(
            "See the README for instructions on adding samples and drum kits!",
            juce::dontSendNotification);
        addAndMakeVisible(emptyLabel);
    }

    viewModel->addListener(this);
    midiCommandManager.addListener(this);
    recordingViewModel->addListener(this);
}

SamplerView::~SamplerView() {
    viewModel->removeListener(this);
    midiCommandManager.removeListener(this);
    recordingViewModel->removeListener(this);
}

void SamplerView::paint(juce::Graphics &g) {}

void SamplerView::resized() {
    titledList.setBounds(getLocalBounds());

    sampleLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                   getHeight() * .1, juce::Font::plain));
    sampleLabel.setBounds(0, getHeight() * .1, getWidth(), getHeight() * .1);

    gainLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                 getHeight() * .075, juce::Font::plain));
    int gainLabelWidth =
        gainLabel.getFont().getStringWidth(gainLabel.getText()) +
        getWidth() * .1;
    int gainLabelHeight = gainLabel.getFont().getHeight();
    int gainLabelPadding = getHeight() * .025;
    gainLabel.setBounds(getWidth() - gainLabelWidth - gainLabelPadding,
                        getHeight() * .025, gainLabelWidth, gainLabelHeight);

    int padding = getWidth() * .1;
    int width = getWidth() - 2 * padding;
    int height = getHeight() * .25;
    int x = padding;
    int y = (getHeight() - height) / 2;

    juce::Rectangle<int> bounds(x, y, width, height);
    fullSampleThumbnail.setBounds(bounds);
    fullSampleThumbnail.setPaintBounds(bounds);
    sampleExcerptThumbnail.setPaintBounds(bounds);

    // Only draw markers if the sample is longer than 0 seconds
    if (viewModel->getFullSampleThumbnail().getTotalLength() > 0) {
        double pixelsPerSecond =
            width / viewModel->getFullSampleThumbnail().getTotalLength();
        double startX = double(x) + viewModel->getStartTime() * pixelsPerSecond;
        double endX = ((viewModel->getEndTime() - viewModel->getStartTime()) *
                       pixelsPerSecond) +
                      startX;
        int startY = (getHeight() - height) / 2;
        juce::Rectangle<int> sampleExcerptThumbnailBounds(
            startX, startY, endX - startX, height);
        sampleExcerptThumbnail.setBounds(sampleExcerptThumbnailBounds);

        juce::Point<float> topLeft(startX, startY);
        juce::Point<float> topRight(startX + 2, startY);
        juce::Point<float> bottomLeft(startX, startY + height);
        juce::Parallelogram<float> markerBounds(topLeft, topRight, bottomLeft);
        startMarker.setRectangle(markerBounds);

        topLeft = juce::Point<float>(endX - 2, startY);
        topRight = juce::Point<float>(endX, startY);
        bottomLeft = juce::Point<float>(endX - 2, startY + height);
        markerBounds =
            juce::Parallelogram<float>(topLeft, topRight, bottomLeft);

        endMarker.setRectangle(markerBounds);
    }

    // Recording thumbnail and time label layout
    recordingThumbnail->setBounds(bounds);
    recordingThumbnail->setPaintBounds(bounds);

    recordingTimeLabel.setFont(
        juce::Font(juce::Font::getDefaultMonospacedFontName(),
                   getHeight() * .1, juce::Font::plain));
    recordingTimeLabel.setBounds(0, getHeight() * .75, getWidth(),
                                 getHeight() * .1);

    emptyLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                                  getHeight() * .1, juce::Font::plain));
    emptyLabel.setBounds(getBounds());
}

void SamplerView::sampleChanged() {
    titledList.getListView().getListBox().selectRow(
        viewModel->itemListState.getSelectedItemIndex());
    sampleLabel.setText(viewModel->getSelectedItemName(),
                        juce::dontSendNotification);
    sendLookAndFeelChange();
    repaint();
    resized();
}

void SamplerView::sampleExcerptTimesChanged() {
    repaint();
    resized();
}

void SamplerView::fullSampleThumbnailChanged() {
    repaint();
    resized();
}

void SamplerView::sampleExcerptThumbnailChanged() {
    repaint();
    resized();
}

void SamplerView::encoder1Increased() {
    if (isShowing()) {
        if (midiCommandManager.getFocusedComponent() == this) {
            if (midiCommandManager.isControlDown) {
                viewModel->toggleSamplePlayDirection();
            } else {
                if (titledList.isVisible())
                    viewModel->increaseSelectedIndex();
            }
        }
    }
}

void SamplerView::encoder1Decreased() {
    if (isShowing()) {
        if (midiCommandManager.getFocusedComponent() == this) {
            if (midiCommandManager.isControlDown) {
                viewModel->toggleSamplePlayDirection();
            } else {
                if (titledList.isVisible())
                    viewModel->decreaseSelectedIndex();
            }
        }
    }
}

void SamplerView::encoder1ButtonReleased() {
    if (isShowing())
        if (midiCommandManager.getFocusedComponent() == this) {
            if (viewModel->getItemNames().size() > 0) {
                if (titledList.isVisible()) {
                    if (viewModel->isDir()) {
                        viewModel->enterDir();
                        titledList.setListItems(viewModel->getItemNames());
                        titledList.setTitleString(viewModel->getTitle());
                    } else {
                        titledList.setVisible(false);
                    }
                } else {
                    titledList.getListView()
                        .getListBox()
                        .scrollToEnsureRowIsOnscreen(
                            viewModel->itemListState.getSelectedItemIndex());
                    titledList.setVisible(true);
                }
            }
        }
}

void SamplerView::encoder2Increased() {
    if (isShowing())
        if (midiCommandManager.getFocusedComponent() == this)
            viewModel->increaseStartTime();
}

void SamplerView::encoder2Decreased() {
    if (isShowing())
        if (midiCommandManager.getFocusedComponent() == this)
            viewModel->decreaseStartTime();
}

void SamplerView::encoder3Increased() {
    if (isShowing())
        if (midiCommandManager.getFocusedComponent() == this)
            viewModel->increaseEndTime();
}

void SamplerView::encoder3Decreased() {
    if (isShowing())
        if (midiCommandManager.getFocusedComponent() == this)
            viewModel->decreaseEndTime();
}

void SamplerView::encoder4Increased() {
    if (isShowing()) {
        if (midiCommandManager.getFocusedComponent() == this) {
            if (midiCommandManager.isControlDown)
                viewModel->increaseGain();
        }
    }
}

void SamplerView::encoder4Decreased() {
    if (isShowing()) {
        if (midiCommandManager.getFocusedComponent() == this) {
            if (midiCommandManager.isControlDown)
                viewModel->decreaseGain();
        }
    }
}

void SamplerView::controlButtonPressed() {
    if (isShowing())
        if (midiCommandManager.getFocusedComponent() == this)
            gainLabel.setVisible(true);
}

void SamplerView::controlButtonReleased() {
    if (isShowing())
        if (midiCommandManager.getFocusedComponent() == this)
            gainLabel.setVisible(false);
}

void SamplerView::gainChanged() {
    if (viewModel->getGain() > 0)
        gainLabel.setText("+" + juce::String(floor(viewModel->getGain())),
                          juce::dontSendNotification);
    else if (viewModel->getGain() == 0)
        gainLabel.setText("-" + juce::String(floor(viewModel->getGain())),
                          juce::dontSendNotification);
    else
        gainLabel.setText(juce::String(floor(viewModel->getGain())),
                          juce::dontSendNotification);

    resized();
}

void SamplerView::noteOnPressed(int noteNumber) {
    if (isShowing())
        if (midiCommandManager.getFocusedComponent() == this)
            viewModel->setSelectedSoundIndex(noteNumber);
}

void SamplerView::recordButtonReleased() {
    if (isShowing()) {
        if (midiCommandManager.getFocusedComponent() == this) {
            if (!currentlyRecording) {
                recordingViewModel->startRecording();
            }
        }
    }
}

void SamplerView::stopButtonReleased() {
    if (isShowing()) {
        if (midiCommandManager.getFocusedComponent() == this) {
            if (currentlyRecording) {
                recordingViewModel->stopRecording();
            }
        }
    }
}

void SamplerView::recordingStateChanged(bool isRecording) {
    currentlyRecording = isRecording;

    if (isRecording) {
        // Show recording UI, hide normal UI
        fullSampleThumbnail.setVisible(false);
        sampleExcerptThumbnail.setVisible(false);
        startMarker.setVisible(false);
        endMarker.setVisible(false);
        sampleLabel.setVisible(false);

        recordingThumbnail->setVisible(true);
        recordingTimeLabel.setVisible(true);
        recordingTimeLabel.setText("REC 0.0s / 30.0s",
                                   juce::dontSendNotification);
    } else {
        // Show normal UI, hide recording UI
        recordingThumbnail->setVisible(false);
        recordingTimeLabel.setVisible(false);

        fullSampleThumbnail.setVisible(true);
        sampleExcerptThumbnail.setVisible(true);
        startMarker.setVisible(true);
        endMarker.setVisible(true);
        sampleLabel.setVisible(true);
    }

    repaint();
}

void SamplerView::recordingTimeChanged(double elapsedSeconds) {
    double remaining =
        recordingViewModel->getMaxRecordingTimeSeconds() - elapsedSeconds;
    juce::String timeText =
        juce::String::formatted("REC %.1fs / %.1fs", elapsedSeconds,
                                recordingViewModel->getMaxRecordingTimeSeconds());
    recordingTimeLabel.setText(timeText, juce::dontSendNotification);
    repaint();
}

void SamplerView::recordingComplete(const juce::File &recordedFile) {
    // Refresh the sample list to include the new recording
    viewModel->refreshSampleList();
    repaint();
    resized();
}
