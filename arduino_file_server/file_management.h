#ifndef FILE_MANAGEMENT_H
#define FILE_MANAGEMENT_H

#include <Arduino.h>
#include <SD.h>
#include <ESPAsyncWebServer.h>
#include "display.h"

extern File uploadFile;
extern String currentPath;
extern bool fileDeletedMessagePending;
extern bool fileUplodedMessagePending; 
extern unsigned long messageStartTime;

bool isFileImage(String fileName);
String listFiles(String path);
String myFolderProcessor(const String& var, String path);
void handleUpload(AsyncWebServerRequest* request, String filename, size_t index, uint8_t *data, size_t len, bool final, String currentPath);
void deleteFile(String filename);
bool deleteFolder(String path);
String getFileIcon(File file);

#endif