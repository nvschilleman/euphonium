#include "BellLogger.h"
#include <memory>
#include <map>
#include "BerryBind.h"
#include "LEDDriver.h"

uint16_t changeUIntScale(uint16_t inum, uint16_t ifrom_min, uint16_t ifrom_max,
                                       uint16_t ito_min, uint16_t ito_max) {
  // guard-rails
  if (ifrom_min >= ifrom_max) {
    return (ito_min > ito_max ? ito_max : ito_min);  // invalid input, return arbitrary value
  }
  // convert to uint31, it's more verbose but code is more compact
  uint32_t num = inum;
  uint32_t from_min = ifrom_min;
  uint32_t from_max = ifrom_max;
  uint32_t to_min = ito_min;
  uint32_t to_max = ito_max;

  // check source range
  num = (num > from_max ? from_max : (num < from_min ? from_min : num));

  // check to_* order
  if (to_min > to_max) {
    // reverse order
    num = (from_max - num) + from_min;
    to_min = ito_max;
    to_max = ito_min;
  }

  // short-cut if limits to avoid rounding errors
  if (num == from_min) return to_min;
  if (num == from_max) return to_max;

  uint32_t result;
  if ((num - from_min) < 0x8000L) {   // no overflow possible
    uint32_t numerator = ((num - from_min) * 2 + 1) * (to_max - to_min + 1);
    result = numerator / ((from_max - from_min + 1) * 2) + to_min;
  } else {    // no pre-rounding since it might create an overflow
    uint32_t numerator = (num - from_min) * (to_max - to_min + 1);
    result = numerator / (from_max - from_min) + to_min;
  }

  return (uint32_t) (result > to_max ? to_max : (result < to_min ? to_min : result));
}

typedef struct gamma_table_t {
  uint16_t to_src;
  uint16_t to_gamma;
} gamma_table_t;

const gamma_table_t gamma_table[] = {   // don't put in PROGMEM for performance reasons
  {    1,      1 },
  {    4,      1 },
  {  209,     13 },
  {  312,     41 },
  {  457,    106 },
  {  626,    261 },
  {  762,    450 },
  {  895,    703 },
  { 1023,   1023 },
  { 0xFFFF, 0xFFFF }          // fail-safe if out of range
};

// simplified Gamma table for Fade, cheating a little at low brightness
const gamma_table_t gamma_table_fast[] = {
  {   384,    192 },
  {   768,    576 },
  {  1023,   1023 },
  { 0xFFFF, 0xFFFF }          // fail-safe if out of range
};


/*********************************************************************************************\
 * Change scales from 8 bits to 10 bits and vice versa
\*********************************************************************************************/
// 8 to 10 to 8 is guaranteed to give the same result
uint16_t change8to10(uint8_t v) {
  return changeUIntScale(v, 0, 255, 0, 1023);
}
// change from 10 bits to 8 bits, but any non-zero input will be non-zero
uint8_t change10to8(uint16_t v) {
  return (0 == v) ? 0 : changeUIntScale(v, 4, 1023, 1, 255);
}

/*********************************************************************************************\
 * Gamma correction
\*********************************************************************************************/
// Calculate the gamma corrected value for LEDS
uint16_t ledGamma_internal(uint16_t v, const struct gamma_table_t *gt_ptr) {
  uint16_t from_src = 0;
  uint16_t from_gamma = 0;

  for (const gamma_table_t *gt = gt_ptr; ; gt++) {
    uint16_t to_src = gt->to_src;
    uint16_t to_gamma = gt->to_gamma;
    if (v <= to_src) {
      return changeUIntScale(v, from_src, to_src, from_gamma, to_gamma);
    }
    from_src = to_src;
    from_gamma = to_gamma;
  }
}
// Calculate the reverse gamma value for LEDS
uint16_t ledGammaReverse_internal(uint16_t vg, const struct gamma_table_t *gt_ptr) {
  uint16_t from_src = 0;
  uint16_t from_gamma = 0;

  for (const gamma_table_t *gt = gt_ptr; ; gt++) {
    uint16_t to_src = gt->to_src;
    uint16_t to_gamma = gt->to_gamma;
    if (vg <= to_gamma) {
      return changeUIntScale(vg, from_gamma, to_gamma, from_src, to_src);
    }
    from_src = to_src;
    from_gamma = to_gamma;
  }
}

// 10 bits in, 10 bits out
uint16_t ledGamma10_10(uint16_t v) {
  return ledGamma_internal(v, gamma_table);
}

// 10 bits resolution, 8 bits in
uint16_t ledGamma10(uint8_t v) {
  return ledGamma10_10(change8to10(v));
}

// Legacy function
uint8_t ledGamma(uint8_t v) {
  if (v < 0) { v = 0; } 
  if (v >= (1<<8)) { v = (1<<8) - 1; };
  return change10to8(ledGamma10(v));
}

// int l_gamma8(uint8_t val) {
//     if (val < 0) { val = 0; }
//     if (val >= (1<<8)) { val = (1<<8) - 1; };
//     int32_t gamma = ledGamma(val);
//     return gamma; 
//     be_pushint(vm, gamma);
//     be_return(vm); // Return
// }

void exportLEDGamma(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("ledGamma", &ledGamma, "led");
    berry->export_function("scale_uint", &changeUIntScale, "led"); 
}
