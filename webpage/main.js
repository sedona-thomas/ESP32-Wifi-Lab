/*
 * main.js: 
 * 
 * @author Sedona Thomas
 */

var esp32BaudRate = 115200;
var port;
var reader;

// Document Click EventListener: triggers when user clicks anywhere on the page
$("#start_button").on('click', async () => {
    removePrompt();
    await setupPort();
    setupReader();
    setupAudio();
    await readLoop();
});

// removePrompt(): removes starting prompt
function removePrompt() {
    let div = document.getElementById('start_prompt');
    div.parentNode.removeChild(div);
}

// setupPort(): asks user to select port
async function setupPort() {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: esp32BaudRate });
}

// setupReader(): sets up a serial port input stream reader
function setupReader() {
    let decoder = new TextDecoderStream();
    inputDone = port.readable.pipeTo(decoder.writable);
    inputStream = decoder.readable;
    reader = inputStream.getReader();
}

// setupAudio(): sets up audio context and initializes modes
function setupAudio() {
    audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    resetModes();
}

// reseteModes(): sets the initial values for the various modes
function resetModes() {
    mode = 'single';
    waveform = 'sine';
    lfo = false;
    numberOfPartials = 5;
    partialDistance = 15;
    modulatorFrequencyValue = 100;
    modulationIndexValue = 100;
    lfoFreq = 2;
}

// readLoop(): reads and processes input stream values
async function readLoop() {
    while (true) {
        const { value, done } = await reader.read();
        string = value;
        if (string.charAt(0) == "{") {
            while (!isJSON(string) && string.length < 1000) {
                const { value, done } = await reader.read();
                string += value;
            }
        }

        if (isJSON(string)) {
            if (done) {
                // Allow the serial port to be closed later.
                console.log("closing connection")
                reader.releaseLock();
                break;
            }
            if (string) {
                data = parseValue(string);
                controlParameters(data);
            }
        }
    }
};

// controlParameters(): manages sensor input to control parameters
function controlParameters(data) {
    redVal = (1 + Math.sin(data["potentiometer_1"])) * (255 / 2);
    greenVal = 60;
    blueVal = 50;
    document.body.style.backgroundColor = 'rgb(' + redVal + ',  ' + greenVal + ', ' + blueVal + ')';
}

// parseValue(): parses value of serial input stream as JSON format
function parseValue(value) {
    if (isJSON(value)) {
        return JSON.parse(value)["data"];
    } else {
        return {}
    }
}

// isJSON(): checks if string is in appropriate JSON format
function isJSON(string) {
    try {
        JSON.parse(string);
    } catch (exception) {
        return false;
    }
    return true;
}

// changeBackgroundColor(): updates the background color for current counter value
function changeBackgroundColor(counterVal) {
    redVal = (1 + Math.sin(counterVal)) * (255 / 2);
    greenVal = 60;
    blueVal = 50;
    document.body.style.backgroundColor = 'rgb(' + redVal + ',  ' + greenVal + ', ' + blueVal + ')';
}