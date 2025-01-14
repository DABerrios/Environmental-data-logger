#ifndef RTC_FUNC_H
#define RTC_FUNC_H
#include <Arduino.h>
#include <RTClib.h>

extern RTC_DS3231 rtc;
void initRTC();
void RTC_get_time(char* time);
void RTC_get_date(char* date, size_t size);

#endif /*RTC_FUNC_H */