#ifndef SENSORS_H
#define SENSORS_H
#include <Arduino.h>

void initTmp102();
float tmp102_read();
void initBME280();
float BME280_temp_read();
float BME280_pressure_read();
float BME280_humidity_read();

#endif /*SENSORS_H */