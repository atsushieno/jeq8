#pragma once

#include "PluginProcessor.h"

class JEQ8AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit JEQ8AudioProcessorEditor(JEQ8AudioProcessor&);
    ~JEQ8AudioProcessorEditor() override;

private:
    void paint(juce::Graphics&) override;
    void resized() override;
    void notifyParameterChangesToWebUI(int32_t index, float newValue);

    juce::GenericAudioProcessorEditor editor {processor};
    juce::String webviewAudioDataURL;
    juce::WebBrowserComponent webview;
    std::unique_ptr<juce::MemoryInputStream> ui_zip_stream;
    std::unique_ptr<juce::ZipFile> ui_zip;
    juce::WebBrowserComponent::Resource audio_web_resource{{}, "application/octet-stream"};
    template<typename T>
    choc::buffer::AllocatedBuffer<T, choc::buffer::SeparateChannelLayout>* getAudioChannelInput(int32_t channel);
};
