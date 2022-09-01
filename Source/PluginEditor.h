/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/Utils/FullSlider.h"

//==============================================================================
/**
*/
class CompressorTestAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CompressorTestAudioProcessorEditor (CompressorTestAudioProcessor&);
    ~CompressorTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CompressorTestAudioProcessor& audioProcessor;

    xynth::FullSlider downThresh, downRatio, attack, decay;
    xynth::FullSlider   upThresh,   upRatio;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorTestAudioProcessorEditor)
};
