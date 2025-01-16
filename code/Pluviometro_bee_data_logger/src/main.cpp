#include <Arduino.h>
#include <BDL.h>
#include <SD.h>
#include <SPI.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include <RTClib.h>
#include <esp_wifi.h>
#include <Wire.h>

/*local includes*/
#include <main.h>
#include <SD_func.h>
#include <sleep_func.h>
#include <RTC_func.h>
#include <wifi_serv_ap.h>
#include <Lora_otaa.h>



BDL bdl;
int color=1;
char timestr[10]="hh:mm:ss";
char date[12]="dd/mm/yyyy";

int RTC_DATA_ATTR sec_to_micro = 1000000;
int RTC_DATA_ATTR sleep_interval = 60 ;
int RTC_DATA_ATTR buckets_counter = 0;
int RTC_DATA_ATTR num_id = 0;
uint32_t RTC_DATA_ATTR counter = 0;
uint32_t RTC_DATA_ATTR counter_limit=10;

Preferences preferences;

void setup() {
  
  Serial.begin(115200);
  while(!Serial){ // wait for serial port to connect. Needed for native USB
    delay(1000);
  }
  
  bdl.begin();
  bdl.setPixelBrightness(255 / 2);
  /*
  bdl.setPixelColor(green);
  delay(1000);
  */
  Wire.begin();
  //initRTC(); // Initalize the RTC
  initwakeup();
  /*
  bdl.setPixelColor(blue);
  delay(1000);
  */
  //touchSleepWakeUpEnable(T3, THRESHOLD);
  wakeup_handler();
  bdl.setPixelColor(purple);
  /*disable bluetooth and wifi*/
  btStop();
  esp_wifi_stop();
  /*
  bdl.setPixelColor(orange);
  delay(1000);
  */
  //initSDCard();
  
}

void loop() {
  if (serverActive) {
    // Check for timeout
      if (millis() - lastActivityTime > TIMEOUT_PERIOD) {
        Serial.println("Timeout reached. Stopping server and going to sleep...");
        server.end();
        serverActive = false;
        WiFi.softAPdisconnect(true);
        goToSleep();
      }
    // Check if the wifi wakeup pin is pressed again to stop the server and sleep
      if (digitalRead(WAKEUP_PIN_WIFI) == HIGH) {
        Serial.println("Stopping server and going to sleep...");// to be removed for final version
        server.end();
        serverActive = false;
        WiFi.softAPdisconnect(true);
        delay(1000);
        goToSleep();
      }     
  }
  else if(loraWANActive){
    os_runloop_once();
  }
  else{
    // If the server is not active, go to sleep
  //os_runloop_once();  
  /*
  bdl.setPixelColor(BDL::colorWheel(color));
  color++;
  delay(5000);
  */
  goToSleep();    
  }  
  
}

void rain_logger(){
  buckets_counter++;
  
  goToSleep();
}

void goToSleep(){
  Serial.println("Going to sleep");
  bdl.setLDO2Power(false);
  esp_deep_sleep_start();
}

void initpreferences(){
    /*initialize the preferences namespace*/
    preferences.begin("log_numb", false);
    /*get data from the preferences objects*/
    num_id = preferences.getInt("s_num", 0);//serial number
    sleep_interval = preferences.getInt("sleep_interval", 60);//data storing sleep interval
    counter = preferences.getUInt("counter", 0);// counter for lora transmission

}


void IRAM_ATTR ISR() {
    bucket_tip();
}

void bucket_tip(){
  Serial.println("Bucket tipped");//remove for final version
  buckets_counter++;  
}