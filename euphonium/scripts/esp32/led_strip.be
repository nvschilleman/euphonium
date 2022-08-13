# Rmt channels
RMT_CHANNEL_0 = 0
RMT_CHANNEL_1 = 1
RMT_CHANNEL_2 = 1
RMT_CHANNEL_3 = 3

# LED Types
LED_WS2812 = 1
LED_SK6812 = 2

class WS2812_GRB_t
  var gamma
  var channel
  var ledc
  # var brightness
  def init(chan, ledt, ledc, pin)
    self.gamma = true
    self.channel = chan
    self.ledc = int(ledc)
    get_native('led', 'create_strip')(self.channel, ledt, self.ledc, pin)
  end

  ### Mapping functions not originating from NeoPixelBus ###

  def scale_uint(num, fmin, fmax, imin, imax)
    return get_native('led', 'scale_uint')(num, fmin, fmax, imin, imax)
  end

  def ledGamma(v)
    return get_native('led', 'led_gamma')(v)
  end

  def berryNeoColor(rgbw, bri)
      bri = (bri != nil) ? bri : 100
      var r = self.scale_uint(bri, 0, 100, 0, (rgbw & 0xFF0000) >> 16)
      var g = self.scale_uint(bri, 0, 100, 0, (rgbw & 0x00FF00) >> 8)
      var b = self.scale_uint(bri, 0, 100, 0, (rgbw & 0x0000FF))
      if self.gamma
        return self.ledGamma(r) << 16 |
              self.ledGamma(g) <<  8 |
              self.ledGamma(b)
      else
        return r << 16 |
              g <<  8 |
              b
      end
  end
  
    ### Low level NeoPixelBus functions ###

  def setPixelColor(index, rgbw, bri)
    get_native('led', 'setPixelColor')(self.channel, index, self.berryNeoColor(rgbw, bri))
  end

  def Show()
    get_native('led', 'Show')(self.channel)
  end

  def Begin()
    get_native('led', 'Begin')(self.channel)
  end

  def Clear()
    get_native('led', 'Clear')(self.channel)
  end

end


test = WS2812_GRB_t(0, 1, 4, 14)
test.Begin()

test.setPixelColor(0,0xFF0000,100)
test.setPixelColor(1,0xEE82EE,50)
test.setPixelColor(2,0x4B0082,40)
test.setPixelColor(3,0xFFFF00,10)

test.Show()

sleep_ms(2000)

test.Clear()

test.setPixelColor(0,0xFF0000,5)
test.setPixelColor(1,0xEE82EE,10)
test.setPixelColor(2,0x4B0082,100)
test.setPixelColor(3,0xFFFF00,50)

test.Show()

sleep_ms(2000)

test.Clear()

test.setPixelColor(0,0xFF0000,50)
test.setPixelColor(1,0xEE82EE,100)
test.setPixelColor(2,0x4B0082,5)
test.setPixelColor(3,0xFFFF00,20)

test.Show()