#include "wifi.h"

void initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting to Wifi..");
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 10);      
  oled.print("Connecting to Wifi..");
  oled.display(); 

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);   
    Serial.print(".");
  }

  Serial.println("");
  // Print the ESP32's IP address 
	Serial.print("ESP32 Web Server's IP address: "); 
	Serial.println(WiFi.localIP()); 

  oled.clearDisplay();
  oled.setCursor(0, 10);   
  oled.setTextColor(WHITE);
  oled.setTextSize(1);    
  oled.println(WiFi.localIP());
  oled.display(); 
}