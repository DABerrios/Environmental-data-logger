#include <Preferences.h>
#include <esp_sleep.h>


/*local files*/
#include <sleep_func.h>
#include <main.h>
#include <wifi_serv_ap.h>
#include <BDL.h>
#include <RTC_func.h>
#include <SD_func.h>
#include <sensors.h>
#include <Lora_otaa.h>
#include <data_processing.h>


bool loraWANActive = false;




void initwakeup(){
  esp_sleep_enable_ext1_wakeup((1ULL<<WAKEUP_PIN_RAIN), ESP_EXT1_WAKEUP_ANY_HIGH);
  //||(1ULL<<WAKEUP_PIN_WIFI)
  esp_sleep_enable_timer_wakeup(sleep_interval * sec_to_micro);
}

void wakeup_handler(){
  

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("Woke up from ext0...");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        //Serial.println("Woke up from ext1...");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        //Serial.println("Woke up from timer...");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        Serial.println("Woke up from touchpad...");
        break;
    default:
        Serial.println("initial boot or unknown wake up reason");
        break;
  }
  uint64_t wakeup_pin_mask = 0;
  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT1:
          wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
          if(wakeup_pin_mask & (1ULL<<WAKEUP_PIN_RAIN)){
            bucket_tip();
            goToSleep();
          }
          /*check if wake up pin is the wifi button pin*/
          else if(wakeup_pin_mask & (1ULL<<WAKEUP_PIN_WIFI)){
          //start the wifi server
            handleWiFiServer();
          }
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      //Serial.println("Woke up from timer...");// to be removed for final version
        attachInterrupt(34, ISR, HIGH);
        // check if the RTC is running 
        initRTC();
        // Initialize the sensors
        initBME280();
        initTmp102();       
        // Initialize the SD card 
        initSDlight();
        
        //log the sensors data and time on the sd card
        handleDataLogging();
        counter++;
        if(counter>=counter_limit){
          counter=0;
          data_processing("/rain_data.txt");
          loraWANActive = true;
          initLoraotaa();
          do_send_ext();
        }
        detachInterrupt(34);
        //set the ESP32 to deep sleep
        goToSleep();
      break;       
    default:
      
      goToSleep();
      break;
  }
}

/**
 * @brief Handles the data logging process.
 *
 * This function performs the following tasks:
 * - Retrieves the current time and date from the RTC.
 * - Reads temperature, humidity, and pressure data from the sensors.
 * - Calculates the amount of rain based on the bucket counter.
 * - Checks if the timer has reached the limit for a daily reset.
 * - Formats the collected data into a string.
 * - Logs the formatted data to a file.
 *
 * @note The function contains debug print statements that should be removed for the final version.
 */
void handleDataLogging() {
    //Serial.println("Woke up for data logging...");// to be removed for final version
    
    char time[10] = "hh:mm:ss";
    float rain=0.0;
    char date[12] = "dd/mm/yyyy";
    float temp;
    float hum;
    float press;
    /*get time and date from RTC*/
    RTC_get_time(time);    
    RTC_get_date(date,sizeof(date));

    /*get Data from sensors*/
    temp= tmp102_read();
    hum= BME280_humidity_read();
    press= BME280_pressure_read();
    
    Serial.println("humidity: " + String(hum));// to be removed for final version
    Serial.println("pressure: " + String(press));// to be removed for final version
    Serial.println("temperature: " + String(temp));// to be removed for final version
    
    /*calculate the rain*/
    rain=buckets_counter*0.0409;
    /*check if the timer has reached the limit(1 day reset)*/
    check_reset_timer();    
    
    char result1[100];    
    snprintf(result1, sizeof(result1), "%.2f,%.2f,%.2f,%.2f,%s,%s,%d", rain, temp, hum, press, time, date, num_id);
    
    
    //Serial.println(result1);// to be removed for final version
    logData("/rain_data.txt", result1);
}

/**
 * @brief Resets the buckets counter if the total sleep time exceeds or equals 24 hours.
 *
 * This function checks if the product of the counter and sleep_interval is greater than or equal to 86400 seconds (24 hours).
 * If the condition is met, it resets the buckets_counter to 0.
 */
void check_reset_timer(){
    if (counter*sleep_interval >= 86400){
    buckets_counter = 0;
    }
}
/**
 * @brief Logs data to a specified file on the SD card.
 *
 * This function opens the specified file in append mode and writes the provided
 * data to it. If the file is successfully opened, the data is written and the file
 * is closed. A delay of 1 second is added after writing the data.
 *
 * @param filename The name of the file to which data will be logged.
 * @param data The data to be written to the file.
 */
void logData(const char *filename, const String &data) {
  //Serial.println("Writing to file 1");// to be removed for final version
  File file = openfile(SD,filename);// Open the file in append mode
  //Serial.println("Writing to file 2");// to be removed for final version
  if (file)
  {
    file.println(data);
    file.close();
    //Serial.println("Data");// to be removed for final version
  }
  delay(1000);
  //Serial.println("Data written to SD: " + data);// to be removed for final version
}