#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h> 
#include <SPI.h>
#include <SD.h>      
#include "display.h"

static const int _CS = D10;
static const int _MOSI = D11;
static const int _MISO = D12;
static const int _SCK = D13;

extern uint64_t card_size_in_gb;

void initSDCard();

#endif