/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompressorTestAudioProcessor::CompressorTestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    treeState(*this, nullptr, "PARAMETER", { createParameterLayout() })
#endif
{
    comp.setAtomics(treeState);
}

CompressorTestAudioProcessor::~CompressorTestAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout CompressorTestAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(ATTACK_ID, "Attack", 1.f, 1000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(DECAY_ID,  "Decay",  1.f, 1000.f, 200.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(DOWN_THRESH_ID, "Downward Threshold", -60.f, 6.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(DOWN_RATIO_ID,  "Downward Ratio", 1.f, 100.f, 4.f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(UP_THRESH_ID, "Upward Threshold", -60.f, 6.f, -30.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(UP_RATIO_ID,  "Upward Ratio", 1.f, 20.f, 4.f));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String CompressorTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CompressorTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CompressorTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CompressorTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CompressorTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CompressorTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CompressorTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CompressorTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CompressorTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void CompressorTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CompressorTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getNumInputChannels();
    spec.sampleRate = sampleRate;

    comp.prepare(spec);
}

void CompressorTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CompressorTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CompressorTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto audioBlock = juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(0, (size_t)totalNumInputChannels);
    auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);

    comp.process(context);

}

//==============================================================================
bool CompressorTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CompressorTestAudioProcessor::createEditor()
{
    return new CompressorTestAudioProcessorEditor (*this);
}

//==============================================================================
void CompressorTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CompressorTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    // 'If' statements are for error checking
    if (xmlState != nullptr) {
        if (xmlState->hasTagName(treeState.state.getType())) {
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CompressorTestAudioProcessor();
}
