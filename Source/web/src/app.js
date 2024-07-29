import { WEQ8Runtime } from "weq8"; // or from "https://cdn.skypack.dev/weq8"
import "weq8/ui"; // or "https://cdn.skypack.dev/weq8/ui"

let audioContext = new AudioContext({sampleRate: 48000});// new OfflineAudioContext(2, 48000 * 10, 48000);
let weq8 = new WEQ8Runtime(audioContext);
// Log the initial state
console.log('Initial audio context state:', audioContext.state);
// FIXME: set appropriate config
let audioBuffer = new AudioBuffer({
    length: 8192,
    numberOfChannels: 2,
    sampleRate: 48000
});
let currentSource = null;
// Log buffer details
console.log('Audio buffer length:', audioBuffer.length);
console.log('Audio buffer duration:', audioBuffer.duration);
weq8.connect(audioContext.destination);

document.querySelector("weq8-ui").runtime = weq8;

let audioRequestURL = document.location.protocol + "//" + document.location.hostname + ":" + document.location.port + "/__audio__";
let audioLoadFailedCount = 0;
let audioLoadSuccessCount = 0;

function updateSamples() {
    // retrieve channel-separate audio source data
    fetch(audioRequestURL).then(response => {
        if (response.ok) {
            response.arrayBuffer().then(buffer => {
                /*
                let bytesPerChannel = buffer.byteLength / sourceNode.channelCount;
                for (let i = 0; i < sourceNode.channelCount; ++i) {
                    let fa = new Float32Array(buffer, bytesPerChannel * i, bytesPerChannel / 4); // 4:sizeof(float)
                    audioBuffer.copyToChannel(fa, i, 0);
                }
                 */
                let audioBuffer = audioContext.createBuffer(2, 8192, audioContext.sampleRate);
                let bytesPerChannel = buffer.byteLength / 2;
                for (let i = 0; i < 2; i++) {
                    let fa = new Float32Array(buffer, bytesPerChannel * i, bytesPerChannel / 4);
                    audioBuffer.copyToChannel(fa, i, 0);
                }

                // Stop the current source if it exists
                if (currentSource) {
                    currentSource.stop();
                    currentSource.disconnect();
                }

                // Create a new source
                currentSource = audioContext.createBufferSource();
                currentSource.buffer = audioBuffer;
                currentSource.loop = true;
                currentSource.connect(weq8.input);

                if (audioContext.state === "suspended") {
                    audioContext.resume().then(() => currentSource.start());
                } else {
                    currentSource.start();
                }
                audioLoadSuccessCount++;
                //if (audioLoadSuccessCount % 10 == 0)
                //    console.log(audioContext.currentTime);
            }, (error) => {
                console.log(error);
            });
        }
        else
            audioLoadFailedCount++;
        if (audioLoadFailedCount === 1)
            console.log("Failed to load audio resource");
    })
}

setInterval(updateSamples, 1000 / 20);
