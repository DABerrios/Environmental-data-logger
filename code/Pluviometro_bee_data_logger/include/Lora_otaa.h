#ifndef LORA_OTAA_H
#define LORA_OTAA_H
#include <Arduino.h>
#include <BDL.h>
#include <Preferences.h>
#include "arduino_lmic.h"

#define SX1276_CS 5
#define SX1276_SCK 6
#define SX1276_MISO 7
#define SX1276_MOSI 8
#define SX1276_RST 9
#define SX1276_DIO0 41
#define SX1276_DIO1 39

void onEvent (ev_t ev);
void printHex2(unsigned v);
void initLoraotaa();
void do_send(osjob_t* j);
void do_send_ext();

#endif /*LORA_OTAA_H */