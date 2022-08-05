#include "LEDDriver.h"
#include "BellUtils.h"
#include "NeoPixelBus-idf/NeoPixelBus.h"

enum {
  WS2812_GRB = 1,
  SK6812_GRBW = 2,

  ledTypeEnd
};

typedef NeoPixelBus<NeoGrbFeature, NeoEsp32RmtN800KbpsMethod> WS2812_GRB_t;
typedef NeoPixelBus<NeoGrbwFeature, NeoEsp32RmtNSk6812Method> SK6812_GRBW_t;

std::map<uint8_t, std::unique_ptr<WS2812_GRB_t>> registeredStrips;

// void setPixelRgb(int RmtChannel, int index, int r, int g, int b) {
//     (*registeredStrips[RmtChannel].get())[index] =
//         Rgb{static_cast<uint8_t>(r), static_cast<uint8_t>(g),
//             static_cast<uint8_t>(b)};
// }


void setPixelRgb(int RmtChannel, int index, int r, int g, int b) {
    registeredStrips[RmtChannel]->SetPixelColor(index, RgbColor(r, g, b));
}

void showPixels(int RmtChannel) { registeredStrips[RmtChannel]->Show(); }

void createStrip(int RmtChannel, int32_t ledType, uint16_t ledCount, uint8_t ledPin) {
    if (ledType < 1) { ledType = 1; }
    if (ledType >= ledTypeEnd) { ledType = ledTypeEnd - 1; }
    if (RmtChannel < 0) { RmtChannel = 0; }

    switch (ledType) {
        case WS2812_GRB:
            registeredStrips.insert(
                {RmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin, (NeoBusChannel) RmtChannel)});
            break;
        case SK6812_GRBW:
            // registeredStrips.insert(
            //     {RmtChannel, std::make_unique<SK6812_GRBW_t>(ledCount, ledPin, (NeoBusChannel) RmtChannel)});
            print('SK6812 Class call');
            break;
    }
}

void exportLEDDriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("create_strip", &createStrip, "led");
    berry->export_function("set_pixel_rgb", &setPixelRgb, "led");
    berry->export_function("Show", &showPixels, "led");
}
