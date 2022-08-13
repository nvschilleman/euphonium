set(COMPONENT_SRCS
    "SPIFFSScriptLoader.cpp"
    "DACAudioOutput.cpp"
    "ScriptsUpdater.cpp"
    "driver/ESP32Platform.cpp"
    "driver/I2CDriver.cpp"
    "driver/I2SDriver.cpp"
    "driver/GPIODriver.cpp"
    "driver/led/LEDDriver.cpp"
    "driver/led/neopixelbus/internal/HsbColor.cpp"
    "driver/led/neopixelbus/internal/HslColor.cpp"
    "driver/led/neopixelbus/internal/HtmlColor.cpp"
    "driver/led/neopixelbus/internal/HtmlColorNames.cpp"
    "driver/led/neopixelbus/internal/HtmlColorNameStrings.cpp"
    "driver/led/neopixelbus/internal/HtmlColorShortNames.cpp"
    "driver/led/neopixelbus/internal/NeoEsp32RmtMethod.cpp"
    "driver/led/neopixelbus/internal/NeoGamma.cpp"
    "driver/led/neopixelbus/internal/NeoPixelAnimator.cpp"
    "driver/led/neopixelbus/internal/Rgb48Color.cpp"
    "driver/led/neopixelbus/internal/RgbColor.cpp"
    "driver/led/neopixelbus/internal/RgbColorBase.cpp"
    "driver/led/neopixelbus/internal/RgbwColor.cpp"
    "driver/led/neopixelbus/internal/SegmentDigit.cpp"
    "driver/Rotary.cpp"
    "driver/WiFiDriver.cpp"
    "driver/dns_server.c"
    "ota/OTAPlugin.cpp"
    "bluetooth/BluetoothPlugin.cpp"
    "bluetooth/BluetoothDriver.cpp"
    "bluetooth/bt_app_core.c"
    "bluetooth/bt_app_sink.c"
    "main.cpp"
)

set(ARDUINO_ARCH_ESP32 "1")
set(COMPONENT_ADD_INCLUDEDIRS "." "driver/." "driver/led/." "driver/led/neopixelbus/." "bluetooth/." "ota/.")
register_component(REQUIRES "euphonium")
component_compile_options(-std=gnu++17)