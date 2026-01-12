#include "VoicePresets.h"
#include <PresetData.h>

namespace internal_plugins {

VoicePreset VoicePresets::presets[128];
juce::String VoicePresets::categories[128];
bool VoicePresets::initialized = false;

void VoicePresets::initializePresets() {
    if (initialized)
        return;

    loadPresetsFromXML();
    initialized = true;
}

void VoicePresets::loadPresetsFromXML() {
    // Load XML from binary data
    auto xmlString = juce::String::createStringFromData(
        PresetData::voice_presets_xml, 
        PresetData::voice_presets_xmlSize
    );
    
    auto xml = juce::XmlDocument::parse(xmlString);
    if (!xml) {
        DBG("Failed to parse voice_presets.xml");
        return;
    }
    
    if (xml->getTagName() != "PresetBank") {
        DBG("Invalid preset bank format");
        return;
    }
    
    // Iterate through categories
    for (auto* categoryElement : xml->getChildWithTagNameIterator("Category")) {
        juce::String categoryName = categoryElement->getStringAttribute("name");
        
        // Iterate through presets in this category
        for (auto* presetElement : categoryElement->getChildWithTagNameIterator("VoicePreset")) {
            int index = presetElement->getIntAttribute("index", -1);
            if (index < 0 || index >= 128)
                continue;
                
            presets[index] = parsePreset(presetElement);
            categories[index] = categoryName;
        }
    }
}

VoicePreset VoicePresets::parsePreset(const juce::XmlElement* presetElement) {
    VoicePreset preset;
    preset.name = presetElement->getStringAttribute("name", "Unnamed");
    
    // Parse oscillators
    if (auto* oscElement = presetElement->getChildByName("Oscillators")) {
        preset.numOscillators = oscElement->getIntAttribute("count", 1);
        
        for (auto* osc : oscElement->getChildWithTagNameIterator("Oscillator")) {
            int idx = osc->getIntAttribute("index", 0);
            if (idx >= 0 && idx < 4) {
                preset.oscillators[idx].waveform = static_cast<Waveform>(osc->getIntAttribute("waveform", 0));
                preset.oscillators[idx].freqMultiplier = static_cast<float>(osc->getDoubleAttribute("freqMultiplier", 1.0));
                preset.oscillators[idx].level = static_cast<float>(osc->getDoubleAttribute("level", 0.5));
                preset.oscillators[idx].phase = static_cast<float>(osc->getDoubleAttribute("phase", 0.0));
                preset.oscillators[idx].numVoices = osc->getIntAttribute("numVoices", 1);
                preset.oscillators[idx].detune = static_cast<float>(osc->getDoubleAttribute("detune", 0.0));
            }
        }
    }
    
    // Parse amp envelope
    if (auto* ampEnv = presetElement->getChildByName("AmpEnvelope")) {
        preset.ampEnvelope.attack = static_cast<float>(ampEnv->getDoubleAttribute("attack", 0.1));
        preset.ampEnvelope.decay = static_cast<float>(ampEnv->getDoubleAttribute("decay", 0.2));
        preset.ampEnvelope.sustain = static_cast<float>(ampEnv->getDoubleAttribute("sustain", 0.7));
        preset.ampEnvelope.release = static_cast<float>(ampEnv->getDoubleAttribute("release", 0.3));
    }
    
    // Parse filter envelope
    if (auto* filterEnv = presetElement->getChildByName("FilterEnvelope")) {
        preset.filterEnvelope.attack = static_cast<float>(filterEnv->getDoubleAttribute("attack", 0.1));
        preset.filterEnvelope.decay = static_cast<float>(filterEnv->getDoubleAttribute("decay", 0.2));
        preset.filterEnvelope.sustain = static_cast<float>(filterEnv->getDoubleAttribute("sustain", 0.7));
        preset.filterEnvelope.release = static_cast<float>(filterEnv->getDoubleAttribute("release", 0.3));
    }
    
    // Parse filter
    if (auto* filter = presetElement->getChildByName("Filter")) {
        preset.filter.type = static_cast<FilterType>(filter->getIntAttribute("type", 0));
        preset.filter.cutoff = static_cast<float>(filter->getDoubleAttribute("cutoff", 2000.0));
        preset.filter.resonance = static_cast<float>(filter->getDoubleAttribute("resonance", 1.0));
        preset.filter.envAmount = static_cast<float>(filter->getDoubleAttribute("envAmount", 0.0));
    }
    
    // Parse LFO
    if (auto* lfo = presetElement->getChildByName("LFO")) {
        preset.lfo.waveform = static_cast<Waveform>(lfo->getIntAttribute("waveform", 0));
        preset.lfo.rate = static_cast<float>(lfo->getDoubleAttribute("rate", 2.0));
        preset.lfo.depth = static_cast<float>(lfo->getDoubleAttribute("depth", 0.0));
        preset.lfo.targetFilterCutoff = lfo->getBoolAttribute("targetFilter", false);
        preset.lfo.targetOscPitch = lfo->getBoolAttribute("targetPitch", false);
        preset.lfo.targetOscLevel = lfo->getBoolAttribute("targetLevel", false);
    }
    
    // Parse macros (supports 7 macros: indices 0-6)
    if (auto* macrosElement = presetElement->getChildByName("Macros")) {
        for (auto* macro : macrosElement->getChildWithTagNameIterator("Macro")) {
            int idx = macro->getIntAttribute("index", 0);
            if (idx >= 0 && idx < 7) {
                preset.macros[idx].name = macro->getStringAttribute("name", "Macro");
                preset.macros[idx].targets.clear();
                
                for (auto* target : macro->getChildWithTagNameIterator("Target")) {
                    MacroTarget t;
                    t.paramType = static_cast<MacroParamType>(target->getIntAttribute("paramType", 0));
                    t.minValue = static_cast<float>(target->getDoubleAttribute("minValue", 0.0));
                    t.maxValue = static_cast<float>(target->getDoubleAttribute("maxValue", 1.0));
                    t.curve = static_cast<MacroCurve>(target->getIntAttribute("curve", 0));
                    preset.macros[idx].targets.add(t);
                }
            }
        }
    }
    
    return preset;
}

const VoicePreset &VoicePresets::getPreset(int index) {
    initializePresets();
    index = juce::jlimit(0, 127, index);
    return presets[index];
}

juce::String VoicePresets::getPresetName(int index) {
    return getPreset(index).name;
}

juce::String VoicePresets::getCategoryName(int index) {
    initializePresets();
    index = juce::jlimit(0, 127, index);
    return categories[index];
}

} // namespace internal_plugins
