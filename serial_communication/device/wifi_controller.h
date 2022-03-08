#ifndef __WIFI_CONTROLLER_H__
#define __WIFI_CONTROLLER_H__

#include <WebServer.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define WAIT 1000

// If your pi is the first and only device connected to the ESP32, should be the
// correct IP by default on raspberry pi
#define CONSOLE_IP "192.168.1.2"
#define CONSOLE_PORT 4210
const char *ssid = "ESP32";
const char *password = "password";
WiFiUDP Udp;

void setupWifi();

#endif