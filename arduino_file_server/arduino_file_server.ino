#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "arduino_secrets.h"
#include <ESPAsyncWebServer.h> 
#include "SD.h"
#include "SPI.h"
#include "web_server_pages/home_page.h"
#include "web_server_pages/upload_file_form.h"
#include "web_server_pages/create-folder.h"

#define OLED_WIDTH 128 
#define OLED_HEIGHT 32
const int _CS = D10;
const int _MOSI = D11;
const int _MISO = D12;
const int _SCK = D13;

uint64_t card_size_in_gb;

bool fileDeletedMessagePending = false;
unsigned long messageStartTime = 0;
bool fileUplodedMessagePending = false;

Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

AsyncWebServer server(80); 

File uploadFile;

String currentPath;

//Init oled
void initOled() {
  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  oled.clearDisplay();
  oled.display();
}

// Init Wifi
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

// Init SD Card
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

  Serial.printf("SD Card Size: %lluGB\n", card_size_in_gb);

  oled.clearDisplay();
  oled.setCursor(0, 10);      
  oled.setTextColor(WHITE);
  oled.setTextSize(1); 
  oled.printf("SD Card Size: %lluGB\n", card_size_in_gb);
  oled.display(); 
}

bool isFileImage(String fileName) {
  fileName.toLowerCase();
  if(
    fileName.endsWith(".jpg")
    || fileName.endsWith(".jpeg")
    || fileName.endsWith(".png")
    || fileName.endsWith(".gif")
  ) {
    return true;
  } else {
    return false;
  }
}

String listFiles(String path) {
  String output = "";
  File root = SD.open(path);
  File file = root.openNextFile();

  while (file) {
    String fileName = file.name();
    double fileSizeToKB = file.size() / 1024.0;
    String fileSize = String(fileSizeToKB) + " KB";

    output += "<tr>";

    if(isFileImage(fileName)) {
      output += "<td><a href='/view?currentDirectory=" + path + "&file=" + fileName + "'>" + fileName + "</a></td>";
    } else if (file.isDirectory()) {
      String folderEmoji = "\xF0\x9F\x93\x81";
      output += "<td>" + folderEmoji + fileName + "</td>";
    } else {
      output += "<td>" + fileName + "</td>";
    }

    output += "<td>";
    if(file.isDirectory()) {
      output += "<a href='/folder-open?currentDirectory=" + path + "&folder=" + fileName + "' class='btn btn-open'>OPEN</a> ";
      output += "<a href='/delete-folder?currentDirectory=" + path + "&folder=" + fileName + "' class='btn btn-delete'>DELETE</a> ";
    } else {
      output += "<a href='/download?currentDirectory=" + path + "&file=" + fileName + "' class='btn btn-download'>DOWNLOAD</a> ";
      output += "<a href='/delete?currentDirectory=" + path + "&file=" + fileName + "' class='btn btn-delete'>DELETE</a> ";
    }

    output += "</td>";
    
    output += "</tr>";
    
    file = root.openNextFile();
  }
  return output;
}

String myFolderProcessor(const String& var, String path) {
  if (var == "FILE_LIST") {
    return listFiles(path);
  } else if (var == "UPLOAD_BUTTON") {
    return "<a href='/upload?currentPath=" + path + "' id='uploadBtn' class='btn'>[ UPLOAD ]</a>";
  } else if (var == "CREATE_FOLDER") {
    return "<a href='/create-folder?currentPath=" + path + "' id='createFolderBtn' class='btn'>[ CREATE FOLDER ]</a>";
  }
  return String();
}

void handleUpload(
  AsyncWebServerRequest* request, 
  String filename, 
  size_t index, 
  uint8_t *data,
  size_t len, 
  bool final,
  String currentPath
  ) {
  if(!index) {
    Serial.printf("UploadStart: %s\n", filename.c_str());
    oled.clearDisplay();
    oled.setCursor(0, 10);     
    oled.setTextColor(WHITE);
    oled.setTextSize(1);  
    oled.println("Upload Start");
    oled.display();

    filename = currentPath + "/" + filename;

    if(SD.exists(filename)) {
      SD.remove(filename);
    }
    uploadFile = SD.open(filename, FILE_WRITE);  
  }

  if(uploadFile) {
    uploadFile.write(data, len);
  }

  if(final) {
    if(uploadFile) {
      uploadFile.close();
      Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index+len);
      oled.clearDisplay();
      oled.setCursor(0, 10);       
      oled.println("File uploaded.");
      oled.display(); 

      fileDeletedMessagePending = true;
      messageStartTime = millis();
    }
  }
}

void deleteFile(String filename) {
  SD.remove(filename);
  Serial.println("File deleted.");

  oled.clearDisplay();
  oled.setCursor(0, 10);       
  oled.println("File deleted.");
  oled.display(); 

  fileDeletedMessagePending = true;
  messageStartTime = millis();
}

bool deleteFolder(String path) {
  File dir = SD.open(path);

  dir.rewindDirectory();

  while(true) {
    File entry = dir.openNextFile();

    if(!entry) {
      break;
    }

    String entryPath = String(path) + "/" + entry.name();

    if(entry.isDirectory()) {
      entry.close();
      deleteFolder(entryPath);
    } else {
      entry.close();
      SD.remove(entryPath.c_str());
    }
  }

  dir.close();

  bool isDeleted = SD.rmdir(path);

  return isDeleted;
}

// Init Wifi Server
void initWifiServer() {
  // Wifi Server endpoints
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { 
    Serial.println("ESP32 Web Server: New request received:"); 
    Serial.println("GET /");       

    request->send_P(200, "text/html", INDEX_HTML, 
        [](const String& var) -> String { 
            return myFolderProcessor(var, "/"); 
        }
    );
  }); 

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/css", PAGE_CSS);
  });

  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");
    Serial.println("GET /upload");

    currentPath = request->getParam("currentPath")->value();

    request->send(200, "text/html", UPLOAD_FILE_FORM);
  });

  server.on("/uploadpage.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/css", UPLOAD_FORM_CSS);
  });

  server.on("/upload", HTTP_POST, 
    [](AsyncWebServerRequest* request) {
      request->redirect("/");
    }, 
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      handleUpload(request, filename, index, data, len, final, currentPath);
    }
  );

  server.on("/download", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");
    Serial.println("GET /download?currentDirectory=currentDirectory?file=fileName");

    if(request->hasParam("currentDirectory") && request->hasParam("file")) {
      String filename = request->getParam("currentDirectory")->value() + "/" + request->getParam("file")->value();

      if(!SD.exists(filename)) {
        request->send(404, "text/plain", "File not found.");
      }
      request->send(SD, filename, "application/octet-stream");
    } else {
      request->send(400, "text/plain", "Missing file and current directory parameters");
    }
  });

  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");
    Serial.println("GET /delete?file=fileName");

    if(request->hasParam("currentDirectory") && request->hasParam("file")) {
      String filename = request->getParam("currentDirectory")->value() + "/" + request->getParam("file")->value();
      
      if(!SD.exists(filename)) {
        request->send(404, "text/plain", "File doesn't exist");
      }

      deleteFile(filename);

      request->redirect("/");  
    } else {
      request->send(400, "text/plain", "No file and current directory parameters");
    }
  });

  server.on("/view", HTTP_GET, [](AsyncWebServerRequest* request) {
    if(request->hasParam("currentDirectory") && request->hasParam("file")) {
      String fileName = request->getParam("currentDirectory")->value() + "/" + request->getParam("file")->value();
     
      if(!SD.exists(fileName)) {
        request->send(404, "text/plain", "File not found");    
      } else {
        String contentType;

        String fileNameToLower = fileName;
        fileNameToLower.toLowerCase();

        if(fileNameToLower.endsWith(".png")) {
          contentType = "image/png";
        } else if (fileNameToLower.endsWith(".gif")) {
          contentType = "image/gif";
        } else if (fileNameToLower.endsWith(".jpeg")){
          contentType = "image/jpeg";
        } else if (fileNameToLower.endsWith(".jpg")) {
          contentType = "image/jpg";
        }

        request->send(SD, fileName, contentType);
      }
    } else {
      request->send(400, "text/plain", "No current directory and file parameters");
    }
  });

  server.on("/create-folder", HTTP_GET, [](AsyncWebServerRequest* request) {
    currentPath = request->getParam("currentPath")->value();

    Serial.println(currentPath);

    request->send_P(200, "text/html", CRATE_FOLDER_FORM);
  });

  server.on("/create_folder_page.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/css", CREATE_FOLDER_FORM_CSS);
  });

  server.on("/create-folder", HTTP_POST, [](AsyncWebServerRequest* request) {
    if(!request->hasParam("folder-name", true)) {
      request->send(400, "text/plain", "Folder name is required.");
    }

    String folderName = currentPath + "/" + request->getParam("folder-name", true)->value();

    if(SD.exists(folderName)) {
      request->send(400, "text/plain", "Folder already exists");
    }

    SD.mkdir(folderName);

    request->redirect("/");
  });

  server.on("/folder-open", HTTP_GET, [](AsyncWebServerRequest* request) {
    if(!request->hasParam("folder") && !request->hasParam("currentDirectory")) {
      request->send(400, "text/plain", "Current directory and folder parameters are requered");
    }

    String folderName = request->getParam("currentDirectory")->value() + "/" + request->getParam("folder")->value();

    if(!SD.exists(folderName)) {
      request->send(404, "text/plain", "Folder doesn't exists");
    }

    request->send_P(200, "text/html", INDEX_HTML, 
        [folderName](const String& var) -> String { 
            return myFolderProcessor(var, folderName); 
        }
    );
  });

  server.on("/delete-folder", HTTP_GET, [](AsyncWebServerRequest* request) {
    if(request->hasParam("currentDirectory") && request->hasParam("folder")) {
      String path = request->getParam("currentDirectory")->value() + "/" + request->getParam("folder")->value();

      if(!SD.exists(path)) {
        request->send(404, "text/plain", "Folder doesn't exists");
      }

      bool isDeleted = deleteFolder(path);

      if(isDeleted) {
        request->redirect("/");
      } else {
        request->send(400, "text/plain", "Deleting folder failed");
      }
    } else {
      request->send(400, "text/plain", "Current directory and folder params are required.");
    }
  });

  server.begin();
}

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





