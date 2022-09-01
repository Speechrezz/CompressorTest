/*
  ==============================================================================

    MyCompressor.h
    Created: 30 Aug 2022 10:07:24am
    Author:  thesp

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace xynth
{
struct CompParams
{
    float thresh   { 1.f };
    float invThresh{ 1.f };
    float invRatio { 1.f };
};

struct CompAtomics
{
    std::atomic<float>* thresh{ nullptr };
    std::atomic<float>* ratio { nullptr };
};

struct CompTimeAtomics
{
    std::atomic<float>* attack{ nullptr };
    std::atomic<float>* decay { nullptr };
};

class UpDownComp {

public:
    UpDownComp() = default;
    ~UpDownComp() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& context);

    void update();

    void setAtomics(juce::AudioProcessorValueTreeState& treeState);

    float processSample(int channel, float sample);

private:
    juce::dsp::BallisticsFilter<float> envelope, spikeRemoverEnv;
    CompParams  paramsDown,  paramsUp;
    CompAtomics atomicsDown, atomicsUp;
    CompTimeAtomics atomicsTime;

    const float loudnessCutoff = 0.000015849f; // -96 dB
    float thing = 0.f;
};
} // namespace xynth