/*-------------------------------------------------------------------------
NeoPixelBus library for ESP-IDF

NeoPixelBus is written by Michael C. Miller.

Conversion to ESP-IDF by Noël Schilleman

-------------------------------------------------------------------------
This file is part of the NeoPixelBus for ESP-IDF Library

NeoPixelBus is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

NeoPixelBus is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with NeoPixel.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

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

#define strncpy_P     std::strncpy
#define strlen_P      std::strlen

#define PROGMEM
#define PGM_P         const char *

#define pgm_read_byte(addr)   (*(const unsigned char *)(addr))
// #define pgm_read_dword(addr void* p)   (*(const uint16_t *)(addr))

extern "C" {
#endif // __cplusplus

// inline uint8_t pgm_read_byte(const void* p) {
//     return *(const uint8_t*)(p);
// }

inline uint16_t pgm_read_dword(const void* p) {
    return *(const uint16_t*)(p);
}

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