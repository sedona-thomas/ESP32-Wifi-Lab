#include "wifi_controller.h"

void setupWifi() {
  IPAddress local_ip(192, 168, 1, 1);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WebServer server(80);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  server.begin();
}

void send(std::string message) {
  Udp.beginPacket(CONSOLE_IP, CONSOLE_PORT);
  Udp.printf(message);
  Udp.endPacket();
  delay(WAIT);
}