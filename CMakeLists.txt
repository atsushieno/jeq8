cmake_minimum_required(VERSION 3.16)

project(JEQ8 VERSION 0.0.1)

set (BaseTargetName JEQ8)

add_subdirectory(external/JUCE)

file(GLOB_RECURSE WebResources Relative "${CMAKE_CURRENT_LIST_DIR}/Source/web/dist-zip"
        "Source/web/dist-zip/*.zip"
)
if (${WebResources} STREQUAL '')
    message("Web resource is not found. Run `npm run build` under `Source/web` first.")
endif()

juce_add_binary_data(${BaseTargetName}_BinaryData
        SOURCES ${WebResources}
)

juce_add_plugin("${BaseTargetName}"
        # VERSION ...                               # Set this if the plugin version is different to the project version
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME "atsushieno"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE AENO
        PLUGIN_CODE XXXX
        LV2URI "http://github.com/atsushieno/jeq8"
        FORMATS AU VST3 LV2 Standalone
        PRODUCT_NAME "${PROJECT_NAME}")

target_sources(${BaseTargetName} PRIVATE
        Source/ProcessorBase.cpp
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_compile_definitions(${BaseTargetName}
        PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1
)

target_link_libraries(${BaseTargetName} PRIVATE
        juce_recommended_config_flags
        juce_recommended_lto_flags
        juce_recommended_warning_flags
        juce::juce_audio_utils
        juce::juce_audio_devices
        juce::juce_gui_extra
        ${BaseTargetName}_BinaryData
)
