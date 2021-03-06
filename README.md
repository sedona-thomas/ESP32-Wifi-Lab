# ESP32-Wifi-Lab

### Setup Arduino

- Download and open the Arduino IDE ([Download](https://www.arduino.cc/en/software))
- Open Preferences 
- Copy and paste the following link ([https://dl.espressif.com/dl/package_esp32_index.json](https://dl.espressif.com/dl/package_esp32_index.json)) into `Additonal Boards Manager URLs` to add the ESP package
- Select `Tools > Boards > Boards Manager`, then search for `esp32` and install the most recent version
- Select `Tools > Boards > ESP32 Arduino > TTGO T1`
- Select `Tools > Manage Libraries`, then search for `TFT_eSPI` and install the most recent version
- In your file storage system, find the Arduino folder
- Open `Arduino/libraries/TFT_eSPI/User_Setup_Select.h`
- Comment out the line that says `#include <User_Setup.h>`
- Uncomment the line that says `#include <User_Setups/Setup25_TTGO_T_Display.h>`

#### Setup Python Serial Ports

- Download Anaconda ([https://www.anaconda.com/products/individual](https://www.anaconda.com/products/individual))
- Open Terminal
- Download pyserial `conda install pyserial`

#### Upload Esp32 Serial Communication

- Open the Arduino IDE
- Open `serial_communication/device/device.ino` from the Arduino IDE
- Plug in the ESP32 TTGO T-Display
- Select `Sketch > Upload`

### Build Hardware

#### Solder Button

- Solder the `+` (voltage in) end of the button to a ground pin of the ESP32
- Solder the `-` (voltage out) end of the button to the `GPIO Pin #37` of the ESP32

#### Solder Potentiometer

- Solder the `V_in` (voltage in, the first pin) end of the potentiometer to a 3V pin of the ESP32 (the 3V pin on the bottom left)
- Solder the `V_out` (voltage out, the second pin) end of the potentiometer to the `GPIO Pin #12` of the ESP32
- Solder the `GND` (ground, the third pin) end of the potentiometer to a ground pin of the ESP32 (the second ground pin on the top left)

#### Solder Joystick

- Solder the `GND` (ground, the first pin) end of the potentiometer to a ground pin of the ESP32 (the ground pin on the bottom right)
- Solder the `+5V` (voltage in, the second pin) end of the potentiometer to a 3V pin of the ESP32 (the 3V pin on the top right)
- Solder the `VRx` (x-axis potentiometer, the third pin) end of the potentiometer to the `GPIO Pin #27` of the ESP32
- Solder the `VRy` (y-axis potentiometer, the fourth pin) end of the potentiometer to the `GPIO Pin #26` of the ESP32
- Solder the `SW` (button, the fifth pin) end of the potentiometer to the `GPIO Pin #25` of the ESP32
