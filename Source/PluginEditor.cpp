#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"
#include "Parameters.h"

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

std::optional<juce::WebBrowserComponent::Resource> getZippedResource(juce::ZipFile* zip, const juce::String& name) {
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

JEQ8AudioProcessorEditor::JEQ8AudioProcessorEditor(
    JEQ8AudioProcessor& p)
    : AudioProcessorEditor(&p),
      webviewAudioDataURL(/*juce::WebBrowserComponent::getResourceProviderRoot()*/ + "/__audio__"),
      webview(juce::WebBrowserComponent{ juce::WebBrowserComponent::Options{}
        .withBackend (juce::WebBrowserComponent::Options::Backend::webview2)
        .withWinWebView2Options (juce::WebBrowserComponent::Options::WinWebView2{})
        .withNativeIntegrationEnabled()
        .withUserScript("")
        .withResourceProvider (
            [this] (const auto& url) {
                if (url == webviewAudioDataURL) {
                    auto jeq8 = dynamic_cast<JEQ8AudioProcessor*>(&processor);
                    assert(jeq8 != nullptr);
                    auto view = jeq8->inputs.getView();
                    auto numBytesPerChannel = view.size.numFrames * sizeof(float);
                    auto numBytes = view.size.numChannels * numBytesPerChannel;
                    if (audio_web_resource.data.size() < numBytes)
                        audio_web_resource.data.resize(numBytes);
                    // copy all channels
                    for (int i = 0, n = view.size.numChannels; i < n; i++) {
                        auto data = view.data.channels[i];
                        memcpy(reinterpret_cast<void *>(audio_web_resource.data.data() + i * numBytesPerChannel), data, numBytesPerChannel);
                    }
                    return std::optional(audio_web_resource);
                }
                return getZippedResource (ui_zip.get(), url);
            },
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

    p.processBlockInvocationListener = [&](JEQ8AudioProcessor* p) {
        // FIXME: implement audio buffer updating or copying
    };
}

JEQ8AudioProcessorEditor::~JEQ8AudioProcessorEditor() {
    ui_zip_stream.reset();
    ui_zip.reset();
}

void JEQ8AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void JEQ8AudioProcessorEditor::resized()
{
    webview.setBounds(getLocalBounds());
}

void JEQ8AudioProcessorEditor::notifyParameterChangesToWebUI(int32_t index, float newValue) {
    int32_t nodeId = index / 5;
    switch (index % 5) {
    case JEQ8_PARAMETER_TOGGLE:
        webview.evaluateJavascript(juce::String::formatted("weq8.toggleBypass(%d, %d)", nodeId, newValue != 0));
        break;
    case JEQ8_PARAMETER_TYPE:
        webview.evaluateJavascript(juce::String::formatted("weq8.setFilterType(%d, %s)", nodeId, EQTypeNames[(int32_t) newValue].toRawUTF8()));
        break;
    case JEQ8_PARAMETER_Q:
        webview.evaluateJavascript(juce::String::formatted("weq8.setFilterQ(%d, %d)", nodeId, newValue));
        break;
    case JEQ8_PARAMETER_GAIN:
        webview.evaluateJavascript(juce::String::formatted("weq8.setFilterGain(%d, %d)", nodeId, newValue));
        break;
    case JEQ8_PARAMETER_FREQUENCY:
        webview.evaluateJavascript(juce::String::formatted("weq8.setFilterFrequency(%d, %d)", nodeId, newValue));
        break;
    }
}
