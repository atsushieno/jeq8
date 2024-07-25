#pragma once

#include "ProcessorBase.h"
#include "Parameters.h"

class NewPluginTemplateAudioProcessor : public PluginHelpers::ProcessorBase
{
    template<typename T>
    void processBlock(juce::AudioBuffer<T>& buffer, juce::MidiBuffer& midiMessages);

public:
    NewPluginTemplateAudioProcessor();

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    void processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    using AudioProcessor::processBlock;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

private:

    Parameters parameters;
};
