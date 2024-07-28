#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

enum EQType {
    none,
    lowpass24,
    highpass12,
    highpass24,
    bandpass,
    lowshelf12,
    lowshelf24,
    highshelf12,
    highshelf24,
    peaking12,
    peaking24,
    notch12,
    notch24
};

inline juce::StringArray EQTypeNames{
    "none",
    "lowpass24",
    "highpass12",
    "highpass24",
    "bandpass",
    "lowshelf12",
    "lowshelf24",
    "highshelf12",
    "highshelf24",
    "peaking12",
    "peaking24",
    "notch12",
    "notch24"
};

using FloatType = float;

//template<typename FloatType>
struct EQControlPoint {
    bool enabled{false};
    EQType type{EQType::none};
    FloatType Q{0};
    FloatType gain{1.0};
    FloatType frequency{0};
};

#define NUM_CONTROL_POINTS 8

#define JEQ8_PARAMETER_TOGGLE 0
#define JEQ8_PARAMETER_TYPE 1
#define JEQ8_PARAMETER_Q 2
#define JEQ8_PARAMETER_GAIN 3
#define JEQ8_PARAMETER_FREQUENCY 4

#define JEQ8_AU_VERSION_HINT 1

//template<typename FloatType>
struct Parameters
{
    std::unique_ptr<juce::AudioParameterBool> createParameter(
        int32_t index, const char* baseName, bool defaultValue) const
    {
        auto name = juce::String::formatted("P%d_%s", index, baseName);
        return std::make_unique<juce::AudioParameterBool>(juce::ParameterID{name, JEQ8_AU_VERSION_HINT}, name, defaultValue);
    }
    template<typename T>
    std::unique_ptr<juce::AudioParameterChoice> createParameter(
        int32_t index, const char* baseName, juce::StringArray& names, int32_t defaultIndex) const
    {
        auto name = juce::String::formatted("P%d_%s", index, baseName);
        return std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{name, JEQ8_AU_VERSION_HINT}, name, names, defaultIndex);
    }
    std::unique_ptr<juce::AudioParameterFloat> createParameter(
        int32_t index, const char* baseName, float defaultValue, float minValue, float maxValue) const
    {
        auto name = juce::String::formatted("P%d_%s", index, baseName);
        return std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{name, JEQ8_AU_VERSION_HINT}, name, minValue, maxValue, defaultValue);
    }

    void add(juce::AudioProcessor* processor) const
    {
        for (auto i = 0, n = NUM_CONTROL_POINTS; i < n; i++) {
            auto groupName = juce::String::formatted("P%d", i);
            auto grp = std::make_unique<juce::AudioProcessorParameterGroup>(groupName, groupName, "_");
            grp->addChild(createParameter(i * 5, "Enabled", false));
            grp->addChild(createParameter<EQType>(i * 5 + JEQ8_PARAMETER_TYPE, "Type", EQTypeNames, EQType::none));
            grp->addChild(createParameter(i * 5 + JEQ8_PARAMETER_Q, "Q", 0.0, 0.1, 18.0));
            grp->addChild(createParameter(i * 5 + JEQ8_PARAMETER_GAIN, "Gain", 0.0, -15.0, 15.0));
            grp->addChild(createParameter(i * 5 + JEQ8_PARAMETER_FREQUENCY, "Frequency", 0.0, 10.0, 24000.0));
            processor->addParameterGroup(std::move(grp));
        }
    }

    struct EQControlPoint eqControlPoints[NUM_CONTROL_POINTS];
};