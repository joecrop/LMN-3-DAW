// clang-format off
/*******************************************************************************
 BEGIN_JUCE_MODULE_DECLARATION
  ID:               internal_plugins
  vendor:           Stone Preston
  version:          1.0.0
  name:             Internal Plugins
  description:      Internal plugins for app
  website:          http://github.com/stonepreston
  license:          GPL-3.0
  dependencies:     juce_data_structures tracktion_engine juce_events juce_core juce_graphics juce_dsp juce_audio_basics
 END_JUCE_MODULE_DECLARATION
*******************************************************************************/
#pragma once

namespace internal_plugins {

    class DrumSamplerPlugin;
    class SuperChordPlugin;

}

#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <tracktion_engine/tracktion_engine.h>
#include <functional>

#include "DrumSamplerPlugin/DrumSamplerPlugin.h"
#include "SuperChordPlugin/VoicePresets.h"
#include "SuperChordPlugin/ChordEngine.h"
#include "SuperChordPlugin/SuperChordSound.h"
#include "SuperChordPlugin/SuperChordVoice.h"
#include "SuperChordPlugin/SuperChordPlugin.h"



