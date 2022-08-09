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

void exportLEDDriver(std::shared_ptr<berry::VmState> berry);
void exportLEDGamma(std::shared_ptr<berry::VmState> berry);

#endif
