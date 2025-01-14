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





void initwakeup(){
  esp_sleep_enable_ext1_wakeup((1ULL<<WAKEUP_PIN_RAIN)|(1ULL<<WAKEUP_PIN_WIFI), ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_sleep_enable_timer_wakeup(sleep_interval * sec_to_micro);
}

void wakeup_handler(){
  

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("Woke up from ext0...");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        Serial.println("Woke up from ext1...");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        Serial.println("Woke up from timer...");
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
      Serial.println("Woke up from timer...");// to be removed for final version
        attachInterrupt(34, ISR, HIGH);
        // check if the RTC is running 
        initRTC();
        // Initialize the sensors
        initBME280();
        initTmp102();       
        // Initialize the SD card 
        initSDCard();
        
        //log the sensors data and time on the sd card
        handleDataLogging();
        counter++;
        if(counter>=counter_limit){
          counter=0;
          //lora_otaa_tx();
        }
        detachInterrupt(34);
        //set the ESP32 to deep sleep
        goToSleep();
      break;       
    default:
      //initRTC();
      break;
  }
}

void handleDataLogging() {
    Serial.println("Woke up for data logging...");// to be removed for final version
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

    /*calculate the rain*/
    rain=buckets_counter*0.0409;
    /*check if the timer has reached the limit(1 day reset)*/
    check_reset_timer();
    
    
    char result1[50];
    snprintf(result1, sizeof(result1), "%.4f,%s,%s,%d", rain, date, time, num_id);
    char result2[100];
    
    snprintf(result2, sizeof(result2), "%.2f,%s", temp, result1);

    //ds18b20.begin();
    //ds18b20.requestTemperatures();
    //float temp = ds18b20.getTempCByIndex(0);
    //Serial.println(temp);
    
    Serial.println(result2);// to be removed for final version
    logData("/rain_data.txt", result2, true);
}

void check_reset_timer(){
    if (counter*sleep_interval >= 86400){
    buckets_counter = 0;
    }
}
void logData(const char *filename, const String &data,bool serialout) {
  initSDCard();
  File file = openfile(SD,filename);// Open the file in append mode
  if (file)
  {
    file.println(data);
    file.close();
  }
  Serial.println("Data written to SD: " + data);// to be removed for final version
}