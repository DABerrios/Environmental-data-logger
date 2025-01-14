#ifndef WIFI_SERV_AP_H
#define WIFI_SERV_AP_H
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

/* Create an AsyncWebServer object on port 80*/
/**
 * @var server
 * @brief AsyncWebServer object instantiated on port 80.
 */
extern AsyncWebServer server;

/* Variable to store received data*/
extern String receivedData;

/* Flag to keep the ESP32 awake when the server is active*/
extern bool serverActive;

/* Network credentials*/
extern const char* ssid;
extern const char* password;

/* Timestamp of the last activity*/
extern unsigned long lastActivityTime;
extern const unsigned long TIMEOUT_PERIOD;

void handleWiFiServer();


#endif /*WIFI_SERV_AP_H */