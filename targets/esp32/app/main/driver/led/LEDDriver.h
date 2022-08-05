#ifndef EUPH_LEDDRIVER_H
#define EUPH_LEDDRIVER_H

#include "BellLogger.h"
#include <memory>
#include <map>
#include "BerryBind.h"
#include "NeoPixelBus-idf/NeoPixelBus.h"

extern std::map<uint8_t, std::unique_ptr<WS2812_GRB_t>> registeredStrips;

void exportLEDDriver(std::shared_ptr<berry::VmState> berry);

#endif
