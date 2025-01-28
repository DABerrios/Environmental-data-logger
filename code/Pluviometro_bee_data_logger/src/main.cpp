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
uint32_t RTC_DATA_ATTR last_line = 0;

Preferences preferences;

/**
 * @brief Setup function for initializing the environmental data logger.
 * 
 * This function performs the following initializations:
 * - Initializes the serial communication at a baud rate of 115200.
 * - Waits for the serial port to connect (necessary for native USB).
 * - Initializes the BDL (presumably a custom library or hardware component).
 * - Sets the pixel brightness to half of the maximum value.
 * - Initializes the I2C communication.
 * - Calls the initwakeup function to set up wakeup configurations.
 * - Calls the wakeup_handler function to handle wakeup events.
 * - Sets the pixel color to purple.
 * - Disables Bluetooth and WiFi to save power.
 */
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

/**
 * @brief Main loop function that handles server activity, LoRaWAN activity, and sleep mode.
 * 
 * This function performs the following tasks:
 * - If the server is active:
 *   - Checks for a timeout and stops the server, disconnects WiFi, and puts the device to sleep if the timeout period is reached.
 *   - Checks if the WiFi wakeup pin is pressed to stop the server, disconnect WiFi, and put the device to sleep.
 * - If LoRaWAN is active, runs the LoRaWAN loop once.
 * - If neither server nor LoRaWAN is active, puts the device to sleep.
 */
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
    goToSleep();    
  }  
  
}

/**
 * @brief Increments the bucket counter and puts the system to sleep.
 * 
 * This function is responsible for logging rain data by incrementing the 
 * bucket counter each time it is called. After updating the counter, it 
 * puts the system into a low-power sleep mode to conserve energy.
 */
void rain_logger(){
  buckets_counter++;
  
  goToSleep();
}

/**
 * @brief Puts the device into deep sleep mode.
 * 
 * This function prints a message to the serial console indicating that the device is going to sleep,
 * disables the LDO2 power using the bdl object, and then starts the ESP deep sleep mode.
 */
void goToSleep(){
  Serial.println("Going to sleep");
  bdl.setLDO2Power(false);
  esp_deep_sleep_start();
}

/**
 * @brief Initializes the preferences namespace and retrieves stored data.
 * 
 * This function initializes the preferences namespace with the name "log_numb"
 * and retrieves the stored values for the serial number, sleep interval, and 
 * LoRa transmission counter. If the values are not found in the preferences, 
 * default values are used.
 * 
 * @details
 * - Initializes the preferences namespace with the name "log_numb".
 * - Retrieves the serial number from preferences, defaulting to 0 if not found.
 * - Retrieves the sleep interval from preferences, defaulting to 60 seconds if not found.
 * - Retrieves the LoRa transmission counter from preferences, defaulting to 0 if not found.
 */
void initpreferences(){
    /*initialize the preferences namespace*/
    preferences.begin("log_numb", false);
    /*get data from the preferences objects*/
    num_id = preferences.getInt("s_num", 0);//serial number
    sleep_interval = preferences.getInt("sleep_interval", 60);//data storing sleep interval
    counter = preferences.getUInt("counter", 0);// counter for lora transmission

}


/**
 * @brief Interrupt Service Routine (ISR) for handling bucket tip events.
 * 
 * This function is marked with IRAM_ATTR to ensure it is placed in the 
 * Internal RAM (IRAM) for faster execution. It is triggered by an 
 * interrupt and calls the bucket_tip() function to handle the event.
 */
void IRAM_ATTR ISR() {
    bucket_tip();
}

/**
 * @brief Handles the event when the bucket tips.
 * 
 * This function is called whenever the bucket tips. It increments the 
 * bucket counter and prints a message to the serial monitor for debugging 
 * purposes. The serial print statement should be removed in the final version.
 */
void bucket_tip(){
  Serial.println("Bucket tipped");//remove for final version
  buckets_counter++;  
}