/*
  ==============================================================================

    MyCompressor.cpp
    Created: 30 Aug 2022 10:07:24am
    Author:  thesp

  ==============================================================================
*/

#include "UpDownComp.h"
#include "../Common/Constants.h"

namespace xynth
{
void UpDownComp::prepare(const juce::dsp::ProcessSpec& spec)
{
    envelope.prepare(spec);
    envelope.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::RMS);

    spikeRemoverEnv.prepare(spec);
    spikeRemoverEnv.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::RMS);
}

void UpDownComp::process(juce::dsp::ProcessContextReplacing<float>& context)
{
    update();

    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    const auto numChannels = outputBlock.getNumChannels();
    const auto numSamples = outputBlock.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* inputSamples = inputBlock.getChannelPointer(channel);
        auto* outputSamples = outputBlock.getChannelPointer(channel);

        thing = 0.f;

        for (size_t i = 0; i < numSamples; ++i)
            outputSamples[i] = processSample(channel, inputSamples[i]);

        //DBG(thing / float(numSamples));
    }
}

void UpDownComp::update()
{
    paramsDown.thresh    = juce::Decibels::decibelsToGain(atomicsDown.thresh->load(std::memory_order_relaxed));
    paramsDown.invThresh = 1.f / paramsDown.thresh;
    paramsDown.invRatio  = 1.f / atomicsDown.ratio->load(std::memory_order_relaxed);

    paramsUp.thresh = juce::Decibels::decibelsToGain(atomicsUp.thresh->load(std::memory_order_relaxed));
    paramsUp.invThresh = 1.f / paramsUp.thresh;
    paramsUp.invRatio = 1.f / atomicsUp.ratio->load(std::memory_order_relaxed);

    const float attack = atomicsTime.attack->load(std::memory_order_relaxed);
    const float decay = atomicsTime.decay->load(std::memory_order_relaxed);

    envelope.setAttackTime (attack);
    envelope.setReleaseTime(decay);

    spikeRemoverEnv.setAttackTime(decay * 2.f);
    spikeRemoverEnv.setReleaseTime(attack * 2.f);
}

void UpDownComp::setAtomics(juce::AudioProcessorValueTreeState& treeState)
{
    atomicsTime.attack = treeState.getRawParameterValue(ATTACK_ID);
    atomicsTime.decay  = treeState.getRawParameterValue(DECAY_ID);

    atomicsDown.thresh = treeState.getRawParameterValue(DOWN_THRESH_ID);
    atomicsDown.ratio  = treeState.getRawParameterValue(DOWN_RATIO_ID);

    atomicsUp.thresh   = treeState.getRawParameterValue(UP_THRESH_ID);
    atomicsUp.ratio    = treeState.getRawParameterValue(UP_RATIO_ID);
}

float UpDownComp::processSample(int channel, float sample)
{
    const auto env = envelope.processSample(channel, sample);

    // Helps midigate awful transients when going from silence, to sound
    auto spikeRemover = spikeRemoverEnv.processSample(
        channel, float(sample > loudnessCutoff));
    spikeRemover = juce::jmap(std::min(spikeRemover, 0.4f), 0.f, 0.4f, 0.f, 1.f);

    float gain = 1.f;
    if (env > paramsDown.thresh) // downward compression
        gain = std::pow(env * paramsDown.invThresh, paramsDown.invRatio - 1.f);
    else if (env < paramsUp.thresh && env > loudnessCutoff) // upwards
    {
        gain = std::pow(env * paramsUp.invThresh, paramsUp.invRatio - 1.f);
        gain = juce::jmap(spikeRemover, 1.f, gain);
    }

    return sample * gain;
}
}