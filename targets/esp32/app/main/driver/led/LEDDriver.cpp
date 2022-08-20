/*-------------------------------------------------------------------------

NeoPixelBerry 
NeoPixelBus functions mapped to the Berry language

NeoPixelBus is written by Michael C. Miller.
Berry mapping by Noël Schilleman

This library contains parts of code taken from Tasmota by Theo Arends
Based on the work of Stephan Hadinger

Intended for use with Euphonium tiny audio platform
https://github.com/feelfreelinux/euphonium

-------------------------------------------------------------------------*/


#include "LEDDriver.h"
#include "LEDGamma.h"
#include "BellUtils.h"
#include "NeoPixelBus.h"


enum {
  WS2812_GRB = 1,
  SK6812_GRBW = 2,

  ledTypeEnd
};

std::map<uint32_t, std::unique_ptr<WS2812_GRB_t>> registeredWS2812;
std::map<uint32_t, std::unique_ptr<SK6812_GRBW_t>> registeredSK6812;

registeredWS2812.merge(registeredSK6812)



//Merging this??

void createStrip(int rmt, int led_type, int led_count, int led_pin) {
    int32_t ledCount = led_count;
    int32_t ledPin = led_pin;
    int32_t ledType = led_type;
    int32_t rmtChannel = rmt;

    if (ledType < 1) { ledType = 1; }
    if (ledType >= ledTypeEnd) { ledType = ledTypeEnd - 1; }
    if (rmtChannel < 0) { rmtChannel = 0; }
    

    registeredLeds.insert(
        // {rmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin)});
        {rmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin, (NeoBusChannel) rmtChannel)});
    
    BELL_LOG(info, "NeoPixelBus", "LED Strip registered");

    // switch (ledType) {
    //     case WS2812_GRB:
    //         registeredLeds.insert(
    //             {rmtChannel, std::make_unique<WS2812_GRB_t>(ledCount, ledPin, (NeoBusChannel) rmtChannel)});
    //         break;
    //     case SK6812_GRBW:
    //         // registeredLeds.insert(
    //         //     {rmtChannel, std::make_unique<SK6812_GRBW_t>(ledCount, ledPin, (NeoBusChannel) rmtChannel)});
    //         print('SK6812 Class call');
    //         break;
    // }

}

void begin(int rmtChannel) {
    BELL_LOG(info, "NeoPixelBus", "Initializing NeoPixelBus object");
    registeredLeds[rmtChannel]->Begin();
}

void show(int rmtChannel) {
    registeredLeds[rmtChannel]->Show();
}

void canShow(int rmtChannel) {
    return registeredLeds[rmtChannel]->CanShow();
}

void isDirty(int rmtChannel) {
    return registeredLeds[rmtChannel]->IsDirty();
}

void dirty(int rmtChannel) {
    registeredLeds[rmtChannel]->Dirty();
}

void pixels(int rmtChannel) {
    uint8_t * pixels;
    pixels = registeredLeds[rmtChannel]->Pixels();

    return pixels;
}

void pixelSize(int rmtChannel) {
    return registeredLeds[rmtChannel]->PixelSize();
}

void pixelCount(int rmtChannel) {
    return registeredLeds[rmtChannel]->PixelCount();
}

void clearTo(int rmtChannel, int col) {
    uint32_t rgbw = col;
    uint8_t w = (rgbw & 0xFF000000) >> 24;
    uint8_t r = (rgbw & 0xFF0000) >> 16;
    uint8_t g = (rgbw & 0xFF00) >> 8;
    uint8_t b = (rgbw & 0xFF);
    
    BELL_LOG(info, "NeoPixelBus", "Clearing LED Strip");
    registeredLeds[rmtChannel]->ClearTo(RgbColor(r, g, b));
}

void setPixelColor(int rmtChannel, int idx, int col) {
    int32_t index = idx;
    uint32_t rgbw = col;
    uint8_t w = (rgbw & 0xFF000000) >> 24;
    uint8_t r = (rgbw & 0xFF0000) >> 16;
    uint8_t g = (rgbw & 0xFF00) >> 8;
    uint8_t b = (rgbw & 0xFF);
    
    BELL_LOG(info, "NeoPixelBus", "Setting LED color");
    registeredLeds[rmtChannel]->SetPixelColor(index, RgbColor(r,g,b));
}

void getPixelColor(int rmtChannel, int idx) {
    int32_t index = idx;
    RgbColor rgb = registeredLeds[rmtChannel]->GetPixelColor(index); 

    BELL_LOG(info, "NeoPixelBus", "Getting LED color");
    return (rgb.R << 16) | (rgb.G << 8) | rgb.B);
}

// Some Tasmota functions which take care of brightness instead of NeopixelBrightnessBus

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
    berry->export_function("NeoPixelBerry", &createStrip, "led");
    berry->export_function("Begin", &begin, "led");
    berry->export_function("Show", &show, "led");
    berry->export_function("CanShow", &canShow, "led");
    berry->export_function("IsDirty", &isDirty, "led");
    berry->export_function("Dirty", &dirty, "led");
    berry->export_function("Pixels", &pixels, "led");
    berry->export_function("PixelSize", &pixelSize, "led");
    berry->export_function("PixelCount", &pixelCount, "led");
    berry->export_function("ClearTo", &clearTo, "led");
    berry->export_function("SetPixelColor", &setPixelColor, "led");
    berry->export_function("GetPixelColor", &getPixelColor, "led");
    berry->export_function("led_gamma", &led_gamma, "led");
    berry->export_function("scale_uint", &scale_uint, "led");  
}
