/*
 * device.ino: reads values from an Esp32 and sends them as serial data
 */

#define BAUDRATE 115200     // baudrate for serial communications
#define DISPLAY_VALUES true // true: sensors; false: rainbow background
#define JSON true           // sends JSON data over serial connection not tagged
#define BUTTON_DELAY 50     // delays button if defined

#include "ValueQueue.h"
#include "esp32_screen.h"
#include "wifi_controller.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <list>
#include <stdint.h>
#include <string>

class Sensor {
protected:
  uint8_t pin;
  uint8_t value;
  ValueQueue values;
  virtual void read() { value = analogRead(pin); };
};

class SerialCommunication {
protected:
  std::string name;
  bool json;
  void send(){};
  void sendWifi(){};
  std::string formatMessage(std::string, uint8_t);
  void sendSerialObject(std::string, uint8_t);
};

class Button : public Sensor, public SerialCommunication {
public:
  Button(){};
  Button(std::string, int, bool);
  Button(int, bool);
  void read();
  void send();
  void sendWifi();
};

class Potentiometer : public Sensor, public SerialCommunication {
public:
  Potentiometer(){};
  Potentiometer(std::string, int, bool);
  Potentiometer(int, bool);
  void read();
  void send();
  void sendWifi();
};

class Joystick : public SerialCommunication {
public:
  Joystick(){};
  Joystick(std::string, int, int, int, bool);
  void send();
  void sendWifi();
  std::string formatMessage(std::string, uint8_t);
  void sendSerialObject();

private:
  Potentiometer potentiometerX;
  Potentiometer potentiometerY;
  Button buttonSW;
};

////////////////////////////////////////////////////////////////////////////////

Button button = Button("b1", 37, JSON);
Potentiometer potentiometer = Potentiometer("p1", 12, JSON);
Joystick joystick = Joystick("j1", 27, 26, 25, JSON);

void setupSerial();
void sendPeripherals();

void setup() {
  setupScreen();
  setupWifi();
}

void loop() {
  updateScreen(DISPLAY_VALUES);
  sendPeripherals();
  delay(FRAMERATE);
}

////////////////////////////////////////////////////////////////////////////////

// setupSerial(): starts serial communication
void setupSerial() {
  Serial.begin(BAUDRATE);
  delay(1000);
}

// sendPeripherals(): sends values of all peripherals
void sendPeripherals() {
  std::string s = "";
  if (JSON) {
    s+= "{ \"data\": {");
    s += button.formatMessage() + ",");
    s += potentiometer.formatMessage() + ",";
    s += joystick.formatMessage();
    s += "}}";
  } else {
    s += "<data>";
    s += button.formatMessage();
    s += potentiometer.formatMessage();
    s += joystick.formatMessage();
    s += "</data>";
  }
  send(s);
}

////////////////////////////////////////////////////////////////////////////////

Button::Button(std::string name_in, int pin_in, bool json_in) {
  name = name_in;
  pin = pin_in;
  value = 0;
#ifdef BUTTON_DELAY
  values = ValueQueue(BUTTON_DELAY);
#endif
  json = json_in;
}

Button::Button(int pin_in, bool json_in) {
  name = "";
  pin = pin_in;
  value = 0;
#ifdef BUTTON_DELAY
  values = ValueQueue(BUTTON_DELAY);
#endif
  json = json_in;
}

// read(): reads button value
void Button::read() {
  pinMode(pin, INPUT_PULLUP);
  values.add(digitalRead(pin));
  value = values.contains(1) ? 1 : 0;
#if DISPLAY_VALUES
  printSensorToScreen("button", value);
#endif
}

// send(): sends data from peripheral over the serial connection
void Button::send() {
  read();
  sendSerialObject("button", value);
}

// sendWifi(): sends data from peripheral over the serial wifi connection
void Button::send() {
  read();
  sendSerialObject("button", value);
}

////////////////////////////////////////////////////////////////////////////////

Potentiometer::Potentiometer(std::string name_in, int pin_in, bool json_in) {
  name = name_in;
  pin = pin_in;
  value = 0;
  json = json_in;
}

Potentiometer::Potentiometer(int pin_in, bool json_in) {
  name = "";
  pin = pin_in;
  value = 0;
  json = json_in;
}

// read(): reads potentiometer value
void Potentiometer::read() {
  values.add(analogRead(pin));
  value = values.median();
#if DISPLAY_VALUES
  printSensorToScreen("potentiometer" + name, value);
#endif
};

// send(): sends data from peripheral over the serial connection
void Potentiometer::send() {
  read();
  sendSerialObject("potentiometer", value);
};

////////////////////////////////////////////////////////////////////////////////

Joystick::Joystick(std::string name_in, int pin_X, int pin_Y, int pin_SW,
                   bool json_in) {
  name = name_in;
  potentiometerX = Potentiometer("x", pin_X, json_in);
  potentiometerY = Potentiometer("y", pin_Y, json_in);
  buttonSW = Button("sw", pin_SW, json_in);
  json = json_in;
}

// send(): sends data from peripheral over the serial connection
void Joystick::send() {
#if DISPLAY_VALUES
  printToScreen("joystick");
#endif
  sendSerialObject();
};

////////////////////////////////////////////////////////////////////////////////

// formatMessage(): formats data from peripheral
std::string SerialCommunication::formatMessage(std::string sensor,
                                               uint8_t value) {
  std::string s = "";
  if (json) {
    if (name.length() > 0) {
      s += "\"" + sensor + "_" + name + "\": ";
    } else {
      s += "\"" + sensor + "\": ";
    }
    s += value;
  } else {
    if (name.length() > 0) {
      s += "<" + sensor + "_" + name + ">";
      s += value;
      s += "</" + sensor + "_" + name + ">";
    } else {
      s += "<" + sensor + ">";
      s += value;
      s += "</" + sensor + ">";
    }
  }
  return s;
}

// send(): sends data from peripheral over the serial connection
void SerialCommunication::sendSerialObject(std::string sensor, uint8_t value) {
  if (json) {
    if (name.length() > 0) {
      Serial.print("\"");
      Serial.print(sensor.c_str());
      Serial.print("_");
      Serial.print(name.c_str());
      Serial.print("\": ");
    } else {
      Serial.print("\"");
      Serial.print(sensor.c_str());
      Serial.print("\": ");
    }
    Serial.print(value);
  } else {
    if (name.length() > 0) {
      Serial.print("<");
      Serial.print(sensor.c_str());
      Serial.print("_");
      Serial.print(name.c_str());
      Serial.print(">");
      Serial.print(value);
      Serial.print("</");
      Serial.print(sensor.c_str());
      Serial.print("_");
      Serial.print(name.c_str());
      Serial.print(">");
    } else {
      Serial.print("<");
      Serial.print(sensor.c_str());
      Serial.print(">");
      Serial.print(value);
      Serial.print("</");
      Serial.print(sensor.c_str());
      Serial.print(">");
    }
  }
}

// formatMessage(): formats data from joystick
std::string Joystick::formatMessage(std::string sensor, uint8_t value) {
  std::string s = "";
  if (json) {
    if (name.length() > 0) {
      s += "\"";
      s += sensor.c_str();
      s += "_";
      s += name.c_str();
      s += "\": ";
    } else {
      s += "\"";
      s += sensor.c_str();
      s += "\": ";
    }
    s += potentiometerX.formatMessage();
    s += potentiometerY.formatMessage();
    s += buttonSW.formatMessage();
  } else {
    if (name.length() > 0) {
      s += "<";
      s += sensor.c_str();
      s += "_";
      s += name.c_str();
      s += ">";
      s += potentiometerX.formatMessage();
      s += potentiometerY.formatMessage();
      s += buttonSW.formatMessage();
      s += "</";
      s += sensor.c_str();
      s += "_";
      s += name.c_str();
      s += ">";
    } else {
      s += "<";
      s += sensor.c_str();
      s += ">";
      s += potentiometerX.formatMessage();
      s += potentiometerY.formatMessage();
      s += buttonSW.formatMessage();
      s += "</";
      s += sensor.c_str();
      s += ">";
    }
  }
  return s;
}

void Joystick::sendSerialObject() {
  if (json) {
    if (name.length() > 0) {
      Serial.print("\"joystick_");
      Serial.print(name.c_str());
      Serial.print("\": ");
    } else {
      Serial.print("\"joystick\": ");
    }
    Serial.print("{");
    potentiometerX.send();
    Serial.print(",");
    potentiometerY.send();
    Serial.print(",");
    buttonSW.send();
    Serial.print("}");
  } else {
    Serial.print("<joystick_");
    Serial.print(name.c_str());
    Serial.print(">");
    potentiometerX.send();
    Serial.print(",");
    potentiometerY.send();
    Serial.print(",");
    buttonSW.send();
    Serial.print("</joystick_");
    Serial.print(name.c_str());
    Serial.print(">");
  }
}