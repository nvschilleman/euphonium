# Rmt channels
RMT_CHANNEL_0 = 0
RMT_CHANNEL_1 = 1
RMT_CHANNEL_2 = 1
RMT_CHANNEL_3 = 3

# LED Types
LED_WS2812 = 1
LED_SK6812 = 2

class WS2812_GRB_t
    var channel
    var ledcount
    # var brightness
    def init(type, pin, ledc, chan)
        self.channel = chan
        self.ledcount = ledc
        get_native('led', 'create_strip')(self.channel, type, self.ledcount, pin)
    end
    
    def setitem(index, item)
        get_native('led', 'set_pixel_rgb')(self.channel, index, item[0], item[1], item[2])
    end

    def Show()
        get_native('led', 'Show')(self.channel)
    end

    def Begin()
        get_native('led', 'Begin')(self.channel)
    end
end

#StatusLedStrip = get_native('led', 'create_strip')(0, 1, 4, 5)


