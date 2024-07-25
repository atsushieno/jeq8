import { WEQ8Runtime } from "weq8"; // or from "https://cdn.skypack.dev/weq8"
import "weq8/ui"; // or "https://cdn.skypack.dev/weq8/ui"

let audioContext = new OfflineAudioContext(2, 48000 * 10, 48000);
let weq8 = new WEQ8Runtime(audioContext);
let sourceNode = audioContext.createBufferSource();
let destinationNode = audioContext.destination;
sourceNode.connect(weq8.input);
weq8.connect(audioContext.destination);

document.querySelector("weq8-ui").runtime = weq8;
