#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

std::string getMimeType(const juce::String& name) {
    if (name.endsWith(".html"))
        return "text/html";
    if (name.endsWith(".js"))
        return "text/javascript";
    if (name.endsWith(".json"))
        return "application/json";
    return "application/octet-stream";
}

const char* getZip(int& size) {
    for (int i = 0, n = BinaryData::namedResourceListSize; i < n; i++) {
        const char* rn = BinaryData::namedResourceList[i];
        auto fn = BinaryData::getNamedResourceOriginalFilename(rn);
        if (juce::String(fn).endsWith(".zip"))
            return BinaryData::getNamedResource(rn, size);
    }
    return nullptr;
}

std::optional<juce::WebBrowserComponent::Resource> getResource(juce::ZipFile* zip, const juce::String& name) {
    for (int i = 0, n = zip->getNumEntries(); i < n; i++) {
        auto entry = zip->getEntry(i);
        auto fn = entry->filename;
        if (name.substring(1) == fn) {
            auto stream = std::unique_ptr<juce::InputStream>(zip->createStreamForEntry(i));
            auto buffer = juce::MemoryBlock(stream->getTotalLength());
            stream->read(buffer.getData(), buffer.getSize());
            auto v = std::vector(reinterpret_cast<std::byte *>(buffer.begin()), reinterpret_cast<std::byte *>(buffer.end()));
            return std::optional{juce::WebBrowserComponent::Resource{v, getMimeType(name)}};
        }
    }
    return std::nullopt;
}

NewPluginTemplateAudioProcessorEditor::NewPluginTemplateAudioProcessorEditor(
    NewPluginTemplateAudioProcessor& p)
    : AudioProcessorEditor(&p),
      webview(juce::WebBrowserComponent{ juce::WebBrowserComponent::Options{}
        .withBackend (juce::WebBrowserComponent::Options::Backend::webview2)
        .withWinWebView2Options (juce::WebBrowserComponent::Options::WinWebView2{})
        .withNativeIntegrationEnabled()
        .withResourceProvider (
            [this] (const auto& url) { return getResource (ui_zip.get(), url); },
            // FIXME: give appropriate URL
            juce::URL { "http://localhost:8080" }.getOrigin())
      })
{
    int size;
    auto zip = (const std::byte*) getZip(size);
    ui_zip_stream = std::make_unique<juce::MemoryInputStream>(zip, size, true);
    ui_zip = std::make_unique<juce::ZipFile>(ui_zip_stream.get(), false);

    addAndMakeVisible(webview);
    setSize(800, 300);
    std::cerr << juce::WebBrowserComponent::getResourceProviderRoot() << std::endl;
    webview.goToURL(juce::WebBrowserComponent::getResourceProviderRoot() + "index.html");
}

NewPluginTemplateAudioProcessorEditor::~NewPluginTemplateAudioProcessorEditor() {
    ui_zip_stream.reset();
    ui_zip.reset();
}

void NewPluginTemplateAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void NewPluginTemplateAudioProcessorEditor::resized()
{
    webview.setBounds(getLocalBounds());
}
