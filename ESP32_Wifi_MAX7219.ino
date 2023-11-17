/*
  Matt Shapiro
  Send text to a MAX7219 LED display via ESP32-hosted webpage over wifi
  
  This sketch is based on the WebSerial library example: ESP32_Demo
  https://github.com/ayushsharma82/WebSerial
*/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Defining size, and output pins
#define MAX_DEVICES 4
#define CS_PIN 5

#define MAX_MSG_LEN 256

char * msgbuffer;

#define LED 2

AsyncWebServer server(80);

const char* ssid = "Matts-ESP32";          // Your WiFi SSID
const char* password = "0987654321";  // Your WiFi Password

MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  len = (len < 256) ? len : 256;
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
  Display.displayClear();
  strcpy(msgbuffer, d.c_str());
  Display.displayScroll(msgbuffer,  PA_RIGHT, PA_SCROLL_LEFT, 150);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  
  WiFi.softAP(ssid, password);

  Display.begin();
  Display.setIntensity(0);
  Display.displayClear();

  msgbuffer = (char*)malloc(MAX_MSG_LEN*sizeof(char));
  strcpy(msgbuffer, WiFi.softAPIP().toString().c_str());

  Serial.print("IP Address: ");
  Serial.println(msgbuffer);

  Display.displayScroll(msgbuffer,  PA_RIGHT, PA_SCROLL_LEFT, 150);

  // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();
}

void loop() {
  //WebSerial.println("Hello!");
  if (Display.displayAnimate()) {
    Display.displayReset();
  }
  delay(20);
}
