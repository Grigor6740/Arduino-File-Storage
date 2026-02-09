#include "USBCDC.h"
#include "file_management.h"
#include "icons.h"

File uploadFile;
String currentPath;
bool fileDeletedMessagePending = false;
bool fileUplodedMessagePending = false;
unsigned long messageStartTime = 0;

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

    String fileIcon = getFileIcon(file);

    if(isFileImage(fileName)) {
      output += "<td>" + fileIcon + " <a href='/view?currentDirectory=" + path + "&file=" + fileName + "'>" + fileName + "</a></td>";
    } else if (file.isDirectory()) {
      output += "<td>" + fileIcon + fileName + "</td>";
    } else {
      output += "<td>" + fileIcon + fileName + "</td>";
    }

    output += "<td>" + fileSize + "</td>";

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

String getFileIcon(File file) {

  if (file.isDirectory()) return ICON_FOLDER;

  String filename = file.name();

  filename.toUpperCase();

  // Documents
  if (filename.endsWith(".PDF")) return ICON_PDF;
  if (filename.endsWith(".DOC") || filename.endsWith(".DOCX")) return ICON_WORD;
  if (filename.endsWith(".TXT") || filename.endsWith(".LOG") || filename.endsWith(".CSV")) return ICON_TEXT;
  if (filename.endsWith(".XLS") || filename.endsWith(".XLSX")) return ICON_EXCEL;

  // Media
  if (filename.endsWith(".JPG") || filename.endsWith(".PNG") || filename.endsWith(".JPEG") || filename.endsWith(".GIF")) return ICON_IMAGE;
  if (filename.endsWith(".MP3") || filename.endsWith(".WAV")) return ICON_AUDIO;

  return ICON_UNK;
}
