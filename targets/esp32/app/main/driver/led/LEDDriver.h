#ifndef EUPH_LEDDRIVER_H
#define EUPH_LEDDRIVER_H

#include "BellLogger.h"
#include <memory>
#include <map>
#include "BerryBind.h"
#include "NeoPixelBus.h"

typedef NeoPixelBus<NeoGrbFeature, NeoEsp32RmtN800KbpsMethod> WS2812_GRB_t;
typedef NeoPixelBus<NeoGrbwFeature, NeoEsp32RmtNSk6812Method> SK6812_GRBW_t;

extern std::map<uint8_t, std::unique_ptr<WS2812_GRB_t>> registeredStrips;

// void setPixelRgb(int RmtChannel, int index, int r, int g, int b);
// void showPixels(int RmtChannel);
// void createStrip(int RmtChannel, int ledType, int ledType, int ledCount, int ledPin);

void exportLEDDriver(std::shared_ptr<berry::VmState> berry);

#endif
