#include "LEDDriver.h"
#include "LEDGamma.h"
#include "BellUtils.h"
#include "NeoPixelBus.h"


enum {
  WS2812_GRB = 1,
  SK6812_GRBW = 2,

  ledTypeEnd
};

// extern typedef NeoPixelBus<NeoGrbFeature, NeoEsp32RmtN800KbpsMethod> WS2812_GRB_t;
// extern typedef NeoPixelBus<NeoGrbwFeature, NeoEsp32RmtNSk6812Method> SK6812_GRBW_t;

std::map<uint32_t, std::unique_ptr<WS2812_GRB_t>> registeredStrips;

// void setPixelRgb(int rmtChannel, int index, int r, int g, int b) {
//     (*registeredStrips[rmtChannel].get()) index =
//         RgbColor{static_cast<uint8_t>(r), static_cast<uint8_t>(g),
//             static_cast<uint8_t>(b)};
// }

void begin(int rmtChannel) {
    BELL_LOG(info, "NeoPixelBus", "Initializing NeoPixelBus object");
    registeredStrips[rmtChannel]->Begin();
}

void showPixels(int rmtChannel) { registeredStrips[rmtChannel]->Show(); }

// void setPixelRgb(int rmtChannel, int index, int r, int g, int b) {
//     registeredStrips[rmtChannel]->SetPixelColor(index, RgbColor(r, g, b));
// }

void setPixelColor(int rmtChannel, int idx, int col) {
    int32_t index = idx;
    uint32_t rgbw = col;
    uint8_t w = (rgbw & 0xFF000000) >> 24;
    uint8_t r = (rgbw & 0xFF0000) >> 16;
    uint8_t g = (rgbw & 0xFF00) >> 8;
    uint8_t b = (rgbw & 0xFF);

            
    BELL_LOG(info, "NeoPixelBus", "Setting LED color");

    registeredStrips[rmtChannel]->SetPixelColor(index, RgbColor(r,g,b));
}

void clearPixels(int rmtChannel) { registeredStrips[rmtChannel]->ClearTo(0); }

void createStrip(int rmt, int led_type, int led_count, int led_pin) {
    int32_t ledCount = led_count;
    int32_t ledPin = led_pin;
    int32_t ledType = led_type;
    int32_t rmtChannel = rmt;

    if (ledType < 1) { ledType = 1; }
    if (ledType >= ledTypeEnd) { ledType = ledTypeEnd - 1; }
    if (rmtChannel < 0) { rmtChannel = 0; }
    
    BELL_LOG(info, "NeoPixelBus", "Created LED strip");
    registeredStrips.insert(
        // {rmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin)});
        {rmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin, (NeoBusChannel) rmtChannel)});
    

    // switch (ledType) {
    //     case WS2812_GRB:
    //         registeredStrips.insert(
    //             {rmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin, (NeoBusChannel) rmtChannel)});
    //         break;
    //     case SK6812_GRBW:
    //         // registeredStrips.insert(
    //         //     {rmtChannel, std::make_unique<SK6812_GRBW_t>(ledCount, ledPin, (NeoBusChannel) rmtChannel)});
    //         print('SK6812 Class call');
    //         break;
    // }

}

int scale_uint(int n,int fmn,int fmx, int imn,int imx){
    uint16_t num = n;
    uint16_t fmin = fmn;
    uint16_t fmax = fmx;
    uint16_t imin = imn;
    uint16_t imax = imx;
    return changeUIntScale(num, fmin, fmax, imin, imax);
}

int led_gamma(int v){
    uint8_t var = v;
    return ledGamma(var);
}

void exportLEDDriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("Begin", &begin, "led");
    berry->export_function("Show", &showPixels, "led");
    berry->export_function("Clear", &clearPixels, "led");
    berry->export_function("setPixelColor", &setPixelColor, "led");
    berry->export_function("create_strip", &createStrip, "led");
    berry->export_function("led_gamma", &led_gamma, "led");
    berry->export_function("scale_uint", &scale_uint, "led");
    // berry->export_function("setPixelRgb", &setPixelRgb, "led");   
}
