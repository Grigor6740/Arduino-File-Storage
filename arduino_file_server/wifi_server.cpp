#include "wifi_server.h"
#include "file_management.h" 
#include "sd_card.h"         

#include "web_server_pages/home_page.h"
#include "web_server_pages/upload_file_form.h"
#include "web_server_pages/create-folder.h"

AsyncWebServer server(80);

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
