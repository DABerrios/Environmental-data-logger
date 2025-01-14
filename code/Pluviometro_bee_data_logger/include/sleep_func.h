#ifndef SLEEP_FUNC_H
#define SLEEP_FUNC_H
#include <Arduino.h>

#define THRESHOLD 40

#define WAKEUP_PIN_RAIN GPIO_NUM_4
#define WAKEUP_PIN_WIFI GPIO_NUM_3

void wakeup_handler();
void check_reset_timer();
void handleDataLogging();
void logData(const char *filename, const String &data,bool serialout);


extern esp_sleep_wakeup_cause_t wakeup_reason;
extern int RTC_DATA_ATTR sec_to_micro;
extern int RTC_DATA_ATTR sleep_interval;



#endif /*SLEEP_FUNC_H */