#include "sd_card.h"

uint64_t card_size_in_gb = 0;

void initSDCard() {
  SPI.begin(_SCK, _MISO, _MOSI, _CS);
  if(!SD.begin(_CS)) {
    Serial.println("Card Mount Failed");
    oled.clearDisplay();
    oled.setCursor(0, 10);    
    oled.setTextColor(WHITE);
    oled.setTextSize(1);   
    oled.print("Card Mount Failed");
    oled.display(); 
    
    return;
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE) {
    Serial.println("No SD Card Attached");
    oled.clearDisplay();
    oled.setCursor(0, 10);    
    oled.setTextColor(WHITE);
    oled.setTextSize(1);   
    oled.print("No SD Card Attached");
    oled.display(); 
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
    oled.clearDisplay();
    oled.setCursor(0, 10);  
    oled.setTextColor(WHITE);
    oled.setTextSize(1);     
    oled.print("SD Card Type: MMC");
    oled.display(); 
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 10);       
    oled.print("SD Card Type: SDSC");
    oled.display(); 
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
    oled.clearDisplay();
    oled.setCursor(0, 10);     
    oled.setTextColor(WHITE);
    oled.setTextSize(1);  
    oled.print("SD Card Type: SDHC");
    oled.display(); 
  } else {
    Serial.println("UNKNOWN");
    oled.clearDisplay();
    oled.setCursor(0, 10);  
    oled.setTextColor(WHITE);
    oled.setTextSize(1);     
    oled.print("SD Card Type: UNKNOWN");
    oled.display(); 
  }

  uint64_t card_size_in_mb = SD.cardSize() / (1024 * 1024);
  card_size_in_gb = card_size_in_mb / 1000;

  uint64_t used_bytes = SD.usedBytes();
  uint64_t used_mbs = used_bytes / (1024.0 * 1024.0);
  uint64_t used_gbs = used_mbs / 1000;

  Serial.printf("Used: %lluGB / Total: %lluGB\n", used_gbs, card_size_in_gb);

  oled.clearDisplay();
  oled.setCursor(0, 10);      
  oled.setTextColor(WHITE);
  oled.setTextSize(1); 
  oled.printf("Used: %lluGB / Total: %lluGB\n", used_gbs, card_size_in_gb);
  oled.display(); 
}