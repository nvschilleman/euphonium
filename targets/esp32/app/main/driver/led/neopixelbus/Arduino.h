/**
 * NeoPixelBerry-IDF 
 * 
 * Port of NeoPixelBus to Berry lang for ESP-IDF
 * 
 * File: Arduino.h
 * Arduino header for ESP32
 */

#pragma once

#include "sdkconfig.h"

#include <cstdint>
#include <cmath>
#include <cstring>
#include <sys/time.h>

// disable SPI code branch
#define __AVR_ATtiny85__

// used in NeoEase.h
#define HALF_PI  M_PI_2
#define PI       M_PI

#ifdef __cplusplus
// used in HtmlColor.h
#include <string>
typedef std::string  String;

#define strncpy_P     strncpy
#define strlen_P      strlen

#define PROGMEM
#define PGM_P         const char *

#define pgm_read_dword(addr) ({ \
  typeof(addr) _addr = (addr); \
  *(const unsigned long *)(_addr); \
})

extern "C" {
#endif // __cplusplus

inline unsigned long micros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return 1000000 * tv.tv_sec + tv.tv_usec;
}

inline unsigned long millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000L) + (tv.tv_usec / 1000L);
}

#ifdef __cplusplus
}
#endif