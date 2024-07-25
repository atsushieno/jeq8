#pragma once

#include "PluginProcessor.h"

class NewPluginTemplateAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit NewPluginTemplateAudioProcessorEditor(NewPluginTemplateAudioProcessor&);
    ~NewPluginTemplateAudioProcessorEditor();

private:
    void paint(juce::Graphics&) override;
    void resized() override;

    juce::GenericAudioProcessorEditor editor {processor};
    juce::WebBrowserComponent webview;
    std::unique_ptr<juce::MemoryInputStream> ui_zip_stream;
    std::unique_ptr<juce::ZipFile> ui_zip;
};
