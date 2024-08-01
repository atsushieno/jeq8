#include "PluginProcessor.h"
#include "PluginEditor.h"

JEQ8AudioProcessor::JEQ8AudioProcessor()
{
    parameters.add(this);
}

void JEQ8AudioProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
    ProcessorBase::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    auto size = choc::buffer::Size::create(getNumInputChannels(), getBlockSize());
    inputs = choc::buffer::ChannelArrayBuffer<float>(size);
}

template<typename T>
void JEQ8AudioProcessor::processBlock(juce::AudioBuffer<T>& buffer,
                                                   juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused(midiMessages);

    // FIXME: implement DSP

    if (typeid(T) ==typeid(double)) {
        // Not supported yet
    } else {
        for (int32_t ch = 0, nCh = getNumInputChannels(); ch < nCh; ++ch) {
            memcpy(inputs.getChannel(ch).data.data, buffer.getReadPointer(ch), buffer.getNumSamples() * sizeof(T));
        }
    }
    if (processBlockInvocationListener != nullptr) {
        processBlockInvocationListener(this);
    }

    /*
    if (parameters.enable->get())
        buffer.applyGain(parameters.gain->get());
    else
        buffer.clear();
    */
}

void JEQ8AudioProcessor::releaseResources() {
}

void JEQ8AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                   juce::MidiBuffer& midiMessages) {
    processBlock<float>(buffer, midiMessages);
}

void JEQ8AudioProcessor::processBlock(juce::AudioBuffer<double>& buffer,
                                                   juce::MidiBuffer& midiMessages) {
    processBlock<double>(buffer, midiMessages);
}

juce::AudioProcessorEditor* JEQ8AudioProcessor::createEditor()
{
    return new JEQ8AudioProcessorEditor(*this);
}

void JEQ8AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    //Serializes your parameters, and any other potential data into an XML:

    juce::ValueTree params("Params");

    for (auto& param: getParameters())
    {
        auto withId = dynamic_cast<juce::AudioProcessorParameterWithID*>(param);
        juce::ValueTree paramTree(withId ? withId->paramID : param->getName(0x100));
        paramTree.setProperty("Value", param->getValue(), nullptr);
        params.appendChild(paramTree, nullptr);
    }

    juce::ValueTree pluginPreset("MyPlugin");
    pluginPreset.appendChild(params, nullptr);
    //This a good place to add any non-parameters to your preset

    copyXmlToBinary(*pluginPreset.createXml(), destData);
}

void JEQ8AudioProcessor::setStateInformation(const void* data,
                                                          int sizeInBytes)
{
    //Loads your parameters, and any other potential data from an XML:

    auto xml = getXmlFromBinary(data, sizeInBytes);

    if (xml != nullptr)
    {
        auto preset = juce::ValueTree::fromXml(*xml);
        auto params = preset.getChildWithName("Params");

        for (auto& param: getParameters())
        {
            auto withId = dynamic_cast<juce::AudioProcessorParameterWithID*>(param);
            auto paramTree = params.getChildWithName(withId ? withId->paramID : param->getName(0x100));

            if (paramTree.isValid())
                param->setValueNotifyingHost(paramTree["Value"]);
        }

        //Load your non-parameter data now
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JEQ8AudioProcessor();
}
