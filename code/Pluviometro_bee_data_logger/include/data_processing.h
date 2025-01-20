#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H
#include <Arduino.h>

void data_processing(const char* fileName);
void readLastPosition();
void saveLastPosition(unsigned long position);
size_t datacomp(const String &line, uint8_t* data);
void datadecomp(const uint8_t* data, size_t size);

#endif /*DATA_PROCESSING_H */