#include "PluginProcessor.h"
#include "PluginEditor.h"

NewPluginTemplateAudioProcessor::NewPluginTemplateAudioProcessor()
{
    parameters.add(*this);
}

template<typename T>
void NewPluginTemplateAudioProcessor::processBlock(juce::AudioBuffer<T>& buffer,
                                                   juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused(midiMessages);

    if (parameters.enable->get())
        buffer.applyGain(parameters.gain->get());
    else
        buffer.clear();
}

void NewPluginTemplateAudioProcessor::releaseResources() {
}

bool NewPluginTemplateAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
    return AudioProcessor::isBusesLayoutSupported(layouts);
}

void NewPluginTemplateAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                   juce::MidiBuffer& midiMessages) {
    processBlock<float>(buffer, midiMessages);
}

void NewPluginTemplateAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer,
                                                   juce::MidiBuffer& midiMessages) {
    processBlock<double>(buffer, midiMessages);
}

juce::AudioProcessorEditor* NewPluginTemplateAudioProcessor::createEditor()
{
    return new NewPluginTemplateAudioProcessorEditor(*this);
}

void NewPluginTemplateAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
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

void NewPluginTemplateAudioProcessor::setStateInformation(const void* data,
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
    return new NewPluginTemplateAudioProcessor();
}
