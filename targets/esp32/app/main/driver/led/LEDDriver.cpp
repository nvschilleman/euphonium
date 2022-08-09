#include "LEDDriver.h"
#include "BellUtils.h"
#include "NeoPixelBus.h"

enum {
  WS2812_GRB = 1,
  SK6812_GRBW = 2,

  ledTypeEnd
};

// extern typedef NeoPixelBus<NeoGrbFeature, NeoEsp32RmtN800KbpsMethod> WS2812_GRB_t;
// extern typedef NeoPixelBus<NeoGrbwFeature, NeoEsp32RmtNSk6812Method> SK6812_GRBW_t;

std::map<uint8_t, std::unique_ptr<WS2812_GRB_t>> registeredStrips;

// void setPixelRgb(int RmtChannel, int index, int r, int g, int b) {
//     (*registeredStrips[RmtChannel].get()) index =
//         RgbColor{static_cast<uint8_t>(r), static_cast<uint8_t>(g),
//             static_cast<uint8_t>(b)};
// }

void begin(int RmtChannel) {
    BELL_LOG(info, "NeoPixelBus", "Initializing NeoPixelBus object");
    registeredStrips[RmtChannel]->Begin();
}

void showPixels(int RmtChannel) { registeredStrips[RmtChannel]->Show(); }

// void setPixelRgb(int RmtChannel, int index, int r, int g, int b) {
//     registeredStrips[RmtChannel]->SetPixelColor(index, RgbColor(r, g, b));
// }

void setPixelColor(int RmtChannel, int index, int r, int g, int b) {
    BELL_LOG(info, "NeoPixelBus", "Setting LED color");
    registeredStrips[RmtChannel]->SetPixelColor(index, RgbColor(r,g,b));
}

void clearPixels(int RmtChannel) { registeredStrips[RmtChannel]->ClearTo(0); }

void createStrip(int RmtChannel, int ledType, int ledCount, int ledPin) {
    if (ledType < 1) { ledType = 1; }
    if (ledType >= ledTypeEnd) { ledType = ledTypeEnd - 1; }
    if (RmtChannel < 0) { RmtChannel = 0; }
    
    BELL_LOG(info, "NeoPixelBus", "Created LED strip");
    registeredStrips.insert(
        // {RmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin)});
        {RmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin, (NeoBusChannel) RmtChannel)});
    

    // switch (ledType) {
    //     case WS2812_GRB:
    //         registeredStrips.insert(
    //             {RmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin, (NeoBusChannel) RmtChannel)});
    //         break;
    //     case SK6812_GRBW:
    //         // registeredStrips.insert(
    //         //     {RmtChannel, std::make_unique<SK6812_GRBW_t>(ledCount, ledPin, (NeoBusChannel) RmtChannel)});
    //         print('SK6812 Class call');
    //         break;
    // }

}

void exportLEDDriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("Begin", &begin, "led");
    berry->export_function("Show", &showPixels, "led");
    berry->export_function("Clear", &clearPixels, "led");
    berry->export_function("setPixelColor", &setPixelColor, "led");
    berry->export_function("create_strip", &createStrip, "led");
    // berry->export_function("setPixelRgb", &setPixelRgb, "led");   
}
