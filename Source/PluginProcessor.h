#pragma once

#include "ProcessorBase.h"
#include "Parameters.h"
#include <audio/choc_SampleBuffers.h>

class JEQ8AudioProcessor : public PluginHelpers::ProcessorBase {
    template<typename T>
    void processBlock(juce::AudioBuffer<T>& buffer, juce::MidiBuffer& midiMessages);

public:
    JEQ8AudioProcessor();

    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    void processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    using AudioProcessor::processBlock;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    // we avoid templating of this std::function here, so
    std::function<void(JEQ8AudioProcessor*)> processBlockInvocationListener{};
    choc::buffer::ChannelArrayBuffer<float> inputs{};

private:

    Parameters parameters;
};
