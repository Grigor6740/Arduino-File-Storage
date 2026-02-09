#include <Wire.h>
#include "wifi.h"
#include "sd_card.h"
#include "display.h"
#include "file_management.h"
#include "wifi_server.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(3000);

  //Init Oled
  initOled();

  //Init Wifi
  initWifi();

  //Init SD card
  initSDCard();

  // Wifi Server
  initWifiServer();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (fileDeletedMessagePending && (millis() - messageStartTime > 10000)) {
    fileDeletedMessagePending = false; 
    
    oled.clearDisplay();
    oled.setCursor(0, 10);       
    oled.printf("SD Card Size: %lluGB\n", card_size_in_gb);
    oled.display(); 
  }

  if(fileUplodedMessagePending && (millis() - messageStartTime > 10000)) {
    fileUplodedMessagePending = false;

    oled.clearDisplay();
    oled.setCursor(0, 10);       
    oled.printf("SD Card Size: %lluGB\n", card_size_in_gb);
    oled.display(); 
  }
}





