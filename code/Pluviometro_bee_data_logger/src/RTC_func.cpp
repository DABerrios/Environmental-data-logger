#include <RTClib.h>
#include <RTC_func.h>
#include <main.h>

RTC_DS3231 rtc;

void initRTC(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");    
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    
  }
  Serial.println("RTC initialized");
}

void RTC_get_time(char* time){
    DateTime now = rtc.now();
    rtc.now().toString(time);
}

void RTC_get_date(char* date, size_t size){
    DateTime now = rtc.now();
    snprintf(date, size, "%02d/%02d/%04d", now.day(), now.month(), now.year());    
}