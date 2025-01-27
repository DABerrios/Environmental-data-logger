
#include <Arduino.h>


#include <SD.h>
#include <SPI.h>
#include <FS.h>

#include <SD_func.h>
#include <main.h>

SPIClass SPI2(HSPI);

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

/**
 * @brief Initializes the SD card and checks its type and size.
 * 
 * This function initializes the SD card using the specified SPI pins and checks if the SD card is properly mounted.
 * It prints the type and size of the SD card to the Serial monitor.
 * 
 * @note This function uses the SPI2 interface for communication with the SD card.
 * 
 * @return void
 */
void initSDCard(){
   SPI2.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS,SPI2))
    {
        Serial.println("SD Card Mount Failed");
        return;
    }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}
/**
 * @brief Initializes the SD card using the specified SPI interface.
 * 
 * This function sets up the SPI interface and attempts to mount the SD card.
 * If the SD card mount fails, it prints an error message to the serial console.
 * If the mount is successful, it prints a success message to the serial console.
 * 
 * @note This function uses the SPI2 interface and specific pin definitions for
 *       SD_SCK, SD_MISO, SD_MOSI, and SD_CS.
 */
void initSDlight(){

    SPI2.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS,SPI2))
    {
        Serial.println("SD Card Mount Failed");
        return;
    }
    Serial.println("Card Mount Success");
}

/**
 * @brief Renames a file on the filesystem.
 * 
 * This function attempts to rename a file from the specified source path to the specified destination path.
 * It prints the result of the operation to the serial monitor.
 * 
 * @param fs Reference to the filesystem object.
 * @param path1 The source file path.
 * @param path2 The destination file path.
 */
void renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

/**
 * @brief Deletes a file from the filesystem.
 * 
 * This function attempts to delete a file specified by the given path from the provided filesystem.
 * It prints a message to the Serial monitor indicating whether the deletion was successful or not.
 * 
 * @param fs Reference to the filesystem object.
 * @param path Path to the file to be deleted.
 */
void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

/**
 * @brief Reads the contents of a file from the filesystem and prints it to the Serial monitor.
 * 
 * This function attempts to open a file at the specified path within the provided filesystem.
 * If the file is successfully opened, it reads the contents of the file and writes them to the
 * Serial monitor. If the file cannot be opened, an error message is printed to the Serial monitor.
 * 
 * @param fs Reference to the filesystem object (e.g., SD, SPIFFS).
 * @param path Path to the file to be read.
 */
void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

/**
 * @brief Opens a file in append mode.
 *
 * This function attempts to open a file at the specified path in append mode using the provided filesystem object.
 * If the file cannot be opened, an error message is printed to the serial output.
 *
 * @param fs Reference to the filesystem object.
 * @param path Path to the file to be opened.
 * @return File object representing the opened file. If the file could not be opened, an invalid File object is returned.
 */
File openfile(fs::FS &fs, const char * path){
    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
      Serial.println("Failed to open file for writing");
    }
    return file;
}
