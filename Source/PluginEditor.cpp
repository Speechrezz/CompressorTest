/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Common/Constants.h"

//==============================================================================
CompressorTestAudioProcessorEditor::CompressorTestAudioProcessorEditor (CompressorTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 500);
    auto& treeState = audioProcessor.treeState;

    downThresh.init(treeState, DOWN_THRESH_ID, "Thresh");
    downRatio.init (treeState, DOWN_RATIO_ID, "Ratio");

    upThresh.init(treeState, UP_THRESH_ID, "Thresh");
    upRatio.init (treeState, UP_RATIO_ID, "Ratio");

    attack.init(treeState, ATTACK_ID, "Attack");
    decay.init (treeState, DECAY_ID, "Decay");

    addAndMakeVisible(downThresh.slider);
    addAndMakeVisible(downRatio.slider);

    addAndMakeVisible(upThresh.slider);
    addAndMakeVisible(upRatio.slider);

    addAndMakeVisible(attack.slider);
    addAndMakeVisible(decay.slider);
}

CompressorTestAudioProcessorEditor::~CompressorTestAudioProcessorEditor()
{
}

//==============================================================================
void CompressorTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void CompressorTestAudioProcessorEditor::resized()
{
    juce::Rectangle<int> rect(0, 100, getWidth(), 100);

    downThresh.slider.setBounds(rect.removeFromLeft(100));
    downRatio.slider.setBounds (rect.removeFromLeft(100));
    attack.slider.setBounds(rect.removeFromLeft(100));
    decay.slider.setBounds (rect.removeFromLeft(100));

    rect.setBounds(0, 300, getWidth(), 100);

    upThresh.slider.setBounds(rect.removeFromLeft(100));
    upRatio.slider.setBounds (rect.removeFromLeft(100));
}
