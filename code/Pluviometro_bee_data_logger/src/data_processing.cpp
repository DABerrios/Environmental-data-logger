#include <SD.h>
#include <SPI.h>
#include <FS.h>

#include <data_processing.h>
#include <main.h>
#include <SD_func.h>
#include <Lora_otaa.h>


char to_send[100];


/**
 * @brief Saves the last position to persistent storage.
 *
 * This function stores the given position value in a persistent storage
 * using the preferences API. The position is saved under the key "last_line".
 *
 * @param position The position value to be saved.
 */
void saveLastPosition(unsigned long position) {
    last_line = position;
    //preferences.putInt("last_line", last_line);
}
/**
 * @brief Reads the last saved position from preferences.
 *
 * This function initializes the preferences with the namespace "last_line"
 * and retrieves the integer value associated with the key "last_line".
 * If the key does not exist, it defaults to 0.
 */
void readLastPosition() {
    preferences.begin("log_numb", false);
    //last_line = preferences.getInt("last_line", 0);
}


/**
 * @brief Processes environmental data from a specified file.
 * 
 * This function reads environmental data from a file on an SD card, calculates
 * average temperature, humidity, and pressure, and prepares a formatted string
 * with the results.
 * 
 * @param fileName The name of the file to read data from.
 * 
 * The function performs the following steps:
 * - Initializes the SD card and reads the last processed position.
 * - Opens the specified file and seeks to the last processed position.
 * - Reads lines from the file into a buffer until the buffer is full or the end of the file is reached.
 * - Parses each line to extract temperature, humidity, and pressure values.
 * - Calculates the average temperature, humidity, and pressure.
 * - Extracts the rain, time, and date from the last line read.
 * - Prepares a formatted string with the rain, average temperature, average humidity, average pressure, time, date, and a unique identifier.
 * 
 * The function assumes that the file contains lines of data in the following format:
 * rain,temperature,humidity,pressure,time,date,id
 * 
 * Example:
 * 0.00,24.52,44.20,101010.86,15:39:10,20/01/2025,11
 */
void data_processing(const char* fileName){
    initSDlight();
    readLastPosition();
    File file = SD.open(fileName, FILE_READ);
    file.seek(last_line);
    

    const size_t max_line_length = 100;
    String linebuffer[max_line_length];
    size_t line_count = 0;

    while (file.available()) {
        String line = file.readStringUntil('\n');        
        linebuffer[line_count++] = line;                
        if (line_count >= max_line_length)
        {
            break;
        }
        
    }
    saveLastPosition(file.position());
    file.close();    
    float avrg_temp = 0;
    float avrg_hum = 0;
    float avrg_press = 0;
    int l_counter = 0;
    for (size_t i = 0; i < line_count; i++) {
        float temp;
        float hum;
        float press;
        sscanf(linebuffer[i].c_str(), "%*f,%f,%f,%f,%*s,%*s,%*d", &temp, &hum, &press);        
        avrg_temp += temp;
        avrg_hum += hum;
        avrg_press += press;
        l_counter++;
    }
    
    avrg_temp /= l_counter;
    avrg_hum /= l_counter;
    avrg_press /= l_counter;       
    float rain = 0.0;
    char time[10] = "hh:mm:ss";
    char date[12] = "dd/mm/yyyy";
    int id = 0;
    sscanf(linebuffer[line_count-1].c_str(), "%f,%*f,%*f,%*f,%[^,],%[^,],%d", &rain, time, date, &id);
    snprintf(to_send, sizeof(to_send), "%.2f,%.2f,%.2f,%.2f,%s,%s,%d", rain, avrg_temp, avrg_hum, avrg_press, time, date, id);
    Serial.println(to_send);
    size_t ind = datacomp(to_send,mydata);
    
    
}

size_t datacomp(const String &line, uint8_t* data) {
    float rain, avrg_temp, avrg_hum, avrg_press;
    int day, month, year, hour, minute, second, id;

    // Parse the CSV line
sscanf(line.c_str(), "%f,%f,%f,%f,%d:%d:%d,%d/%d/%d,%d", &rain, &avrg_temp, &avrg_hum, &avrg_press, &hour, &minute, &second, &day, &month, &year, &id);    Serial.println(year);
    Serial.println(month);
    Serial.println(day);
    Serial.println(hour);
    Serial.println(minute);
    Serial.println(second);
    Serial.println(id);
    // Convert data to compact binary format
    uint16_t rainInt = (uint16_t)(rain * 100);          
    uint16_t tempInt = (uint16_t)(avrg_temp * 100);     
    uint16_t humInt = (uint16_t)(avrg_hum * 100);       
    uint32_t pressInt = (uint32_t)(avrg_press * 100);   
    uint16_t dateInt = ((year - 2000) << 9) | (month << 5) | day;  
    uint16_t timeInt = (hour << 11) | (minute << 5) | (second / 2); 
    uint8_t idInt = (uint8_t)id;                       // ID fits in 1 byte

    // Pack the data into the array
    size_t index = 0;
    data[index++] = rainInt >> 8; data[index++] = rainInt & 0xFF;
    data[index++] = tempInt >> 8; data[index++] = tempInt & 0xFF;
    data[index++] = humInt >> 8; data[index++] = humInt & 0xFF;
    data[index++] = (pressInt >> 16) & 0xFF;
    data[index++] = (pressInt >> 8) & 0xFF;
    data[index++] = pressInt & 0xFF;
    data[index++] = dateInt >> 8; data[index++] = dateInt & 0xFF;
    data[index++] = timeInt >> 8; data[index++] = timeInt & 0xFF;
    data[index++] = idInt;

    

    return index;  // Return the total size of the packed data
}

/**
 * @brief Decompresses and decodes environmental data from a byte array.
 *
 * This function takes a byte array containing compressed environmental data and
 * decodes it into human-readable values such as rain, temperature, humidity, pressure,
 * date, time, and an ID. The data is expected to be in a specific format and order.
 *
 * @param data Pointer to the byte array containing the compressed data.
 * @param size Size of the byte array. Must be at least 14 bytes.
 *
 * The expected format of the data array is as follows:
 * - 2 bytes for rain (uint16_t, scaled by 100)
 * - 2 bytes for temperature (uint16_t, scaled by 100)
 * - 2 bytes for humidity (uint16_t, scaled by 100)
 * - 3 bytes for pressure (uint32_t, scaled by 100)
 * - 2 bytes for date (uint16_t, encoded as day, month, year)
 * - 2 bytes for time (uint16_t, encoded as hour, minute, second)
 * - 1 byte for ID (uint8_t)
 *
 * The decoded values are printed to the serial output.
 *
 * @note If the size of the data array is less than 14 bytes, an error message is printed
 *       and the function returns without processing the data.
 */
void datadecomp(const uint8_t* data, size_t size) {
    if (size < 14) {
        Serial.println("Error: Insufficient data size");
        return;
    }

    size_t index = 0;

    // Decode Rain
    uint16_t rainInt = (data[index++] << 8) | data[index++];
    float rain = rainInt / 100.0;  // Convert back to float (mm)

    // Decode Temperature
    uint16_t tempInt = (data[index++] << 8) | data[index++];
    float temperature = tempInt / 100.0;  // Convert back to float (°C)

    // Decode Humidity
    uint16_t humInt = (data[index++] << 8) | data[index++];
    float humidity = humInt / 100.0;  // Convert back to float (%)

    // Decode Pressure
    uint32_t pressInt = (data[index++] << 16) | (data[index++] << 8) | data[index++];
    float pressure = pressInt / 100.0;  // Convert back to float (Pa)

    // Decode Date
    uint16_t dateInt = (data[index++] << 8) | data[index++];
    uint8_t day = dateInt & 0x1F;
    uint8_t month = (dateInt >> 5) & 0x0F;
    uint16_t year = (dateInt >> 9) + 2000;            

    // Decode Time
    uint16_t timeInt = (data[index++] << 8) | data[index++];
    uint8_t hour = timeInt >> 11;         // Extract hour (5 bits)
    int minute = (timeInt >> 5) & 0x3F;         // Extract minute (6 bits)
    int second = (timeInt & 0x1F) * 2;          // Extract second (5 bits, multiply by 2)

    // Decode ID
    uint8_t id = data[index++];

    // Print the decoded values
}