#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <BDL.h>
#include <Preferences.h>

extern BDL bdl;
extern Preferences preferences;

void goToSleep();
void rain_logger();
void calibrateTouch();
void initwakeup();
void IRAM_ATTR ISR();
void bucket_tip();

extern int RTC_DATA_ATTR num_id;
extern int RTC_DATA_ATTR sec_to_micro;
extern int RTC_DATA_ATTR sleep_interval;
extern int RTC_DATA_ATTR buckets_counter;
extern uint32_t RTC_DATA_ATTR counter;
extern uint32_t RTC_DATA_ATTR counter_limit;
#endif /*MAIN_H */