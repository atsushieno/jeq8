# What is this?

It is a proof-of-concept JUCE-based audio plugin that -

- builds on top of WebView (`WebBrowserComponent`)
- whose `PluginProcessorEditor` interacts with audio inputs that is passed
  as `audioBuffer` at `AudioProcessor::processBlock()`
- reflects the inputs on to the UI as waveform, using WebAudio AnalyserNode.

It is nothing but an experiment. It even does not perform EQ audio processing because it is just a demonstration of Web UI, and I'm just reusing an existing WebAudio based project [teropa/weq8](https://github.com/teropa/weq8) to achieve a practical plugin UI.

The audio input is simply pass-through and it is passed to the UI as is, as
it does not matter to this conceptual project.

(If I want to achieve DSP, I would have to "port" existing `WEQ8Runtime` part of the original project to C++. I quickly dumped the idea as it is not what I wanted to achieve.)

## Why does this matter?

The primary goal (actually my sole purpose) here is, to prove that Web UI works for the audio inputs, not just control events, to some extent. Web UI is cool, but a widely common concern is that it would not achieve performant rendering when audio inputs are involved. To be honest I did not believe in that it is doable either.

This particular application is based on WebAudio API without audio processing on the Web side.


## License

JEQ8 is licensed under the AGPLv3 license.

We use [weq8](https://github.com/teropa/weq8) and it is distributed under the ISC license.

We use [choc](https://github.com/Tracktion/choc/) and it is distributed under the ISC license.

