#ifndef SD_FUNC_H
#define SD_FUNC_H
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <FS.h>

void initSDCard();
void appendFile(fs::FS &fs, const char * path, const char * message);
void writeFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void readFile(fs::FS &fs, const char * path);
void deleteFile(fs::FS &fs, const char * path);
File openfile(fs::FS &fs, const char * path);

#endif /*SD_FUNC_H */