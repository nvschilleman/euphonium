#pragma once

// For those platforms/methods that support dynamic channel setting
enum NeoBusChannel
{
    NeoBusChannel_0,
    NeoBusChannel_1,
    NeoBusChannel_2,
    NeoBusChannel_3,
#if !defined(CONFIG_IDF_TARGET_ESP32S2)
    NeoBusChannel_4,
    NeoBusChannel_5,
    NeoBusChannel_6,
    NeoBusChannel_7,
#endif // CONFIG_IDF_TARGET_ESP32S2
};