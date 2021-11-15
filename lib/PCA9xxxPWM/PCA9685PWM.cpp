/*
  The MIT License (MIT)
  Copyright (c) Kiyo Chinzei (kchinzei@gmail.com)
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
/*
  PCA9xxxPWM - Arduino / C++ class for NXP PCA9xxx I2C connected PWM devices.

  Make Asayake to Wake Project.
  Kiyo Chinzei
  https://github.com/kchinzei/PCA9xxxPWM
*/

#include "PCA9685PWM.h"

/*
  PCA9685 can offset 'on timing' to decrease the current surge and EMC.
  Offset will be given so that the distance of each channel being as large as possible,
  assuming that we use channels from 0 to 15, board address given by wiring from 0,1,..
    offset = (baseClockHertz / frequency) / maxChannels * onOffset_Ch[ch]
*/
const uint8_t onOffsetPerCh[] = {
  0x00, 0x08, 0x04, 0x0C,
  0x02, 0x0A, 0x06, 0x0E,
  0x01, 0x09, 0x05, 0x0D,
  0x03, 0x0B, 0x07, 0x0F
};

const uint8_t onOffsetPerBoard[] = {
  0x00, 0x20, 0x10, 0x30,
  0x08, 0x28, 0x18, 0x38,
  0x04, 0x34, 0x14, 0x34,
  0x0C, 0x2C, 0x1C, 0x3C,

  0x02, 0x22, 0x12, 0x32,
  0x0A, 0x2A, 0x1A, 0x3A,
  0x06, 0x26, 0x16, 0x36,
  0x0E, 0x2E, 0x1E, 0x3E,

  0x01, 0x21, 0x11, 0x31,
  0x09, 0x29, 0x19, 0x39,
  0x05, 0x25, 0x15, 0x35,
  0x0D, 0x2D, 0x1D, 0x3D,

  0x03, 0x23, 0x13, 0x33,
  0x0B, 0x2B, 0x1B, 0x3B,
  0x07, 0x27, 0x17, 0x37,
  0x0F, 0x2F, 0x1F, 0x3F
};

enum {
  REGISTERS_PER_PORT = 4,
  BASECLOCK_HERTZ = 25000000,
  DEFAULT_ADDRESS = 0x40, // H/W fixed.
  STEPS = 4096,
  MIN_PRESCALE = 0x03,
  MAX_PRESCALE = 0xFF,
};

/*
  It's a simple counting of 6 bit addressing.
  In reality at least 0xE0 (AllCall) unusable.
  And I2C special purpose addresses should not be used.
  https://www.i2c-bus.org/addressing/
 */
#define MAX_BOARDS (64.0 /* float on purpose */)
#define MAX_PORTS 16

PCA9685PWM::PCA9685PWM(uint8_t i2cAddr, TwoWire *i2cPort)
  : PCA9xxxPWM(i2cAddr, i2cPort), n_of_ports(MAX_PORTS) {}

PCA9685PWM::~PCA9685PWM() {}

boolean PCA9685PWM::begin() {
  if (PCA9xxxPWM::isConnected(_i2cAddr, _i2cPort) &&
      PCA9685PWM::isMyDevice(_i2cAddr, _i2cPort)) {
    return PCA9xxxPWM::begin();
  } else {
    return false;
  }
}

void PCA9685PWM::init(void) {
  uint8_t init_array0[] = {
      PCA9685PWM::REGISTER_START, // Reg Addr
      PCA9685PWM::MODE1_AI |
      PCA9685PWM::MODE1_ALLCALL,  // MODE1
      PCA9685PWM::MODE2_INVRT     // MODE2
  };
  uint8_t init_array1[] = {
      PCA9685PWM::ALL_LED_OFF_H, 
      0x10, // ALL_LED full OFF
  };

  write(init_array0, sizeof(init_array0));
  write(init_array1, sizeof(init_array1));
}

void PCA9685PWM::pwm(uint8_t port, float v) {
  uint8_t val[5] = {pwm_register_access(port), 0,0,0,0};
  uint16_t dutyCycle = simple_exp(v) * STEPS;

  if (dutyCycle >= STEPS) {
    val[2] = 0x10;
  } else if (dutyCycle <= 0) {
    val[4] = 0x10;
  } else {
    uint8_t ch = port >= n_of_ports? 0 : port;
    uint8_t board = _i2cAddr - DEFAULT_ADDRESS;
    
    uint16_t onStep = STEPS / MAX_PORTS * (onOffsetPerCh[ch] + onOffsetPerBoard[board] / MAX_BOARDS);
    uint16_t offStep = onStep + dutyCycle;
    if (offStep > STEPS) offStep -= STEPS;
    val[1] = onStep & 0xFF;
    val[2] = (onStep & 0x0F00) >> 8;
    val[3] = offStep & 0xFF;
    val[4] = (offStep & 0x0F00) >> 8;
  }

  write(val, sizeof(val));
}

void PCA9685PWM::pwm(float *vp) {
  for (uint8_t i=0; i<n_of_ports; i++) {
    pwm(i, *vp++);
  }
}

void PCA9685PWM::freq(float freq, float ext_clock) {
  if (freq > 0) {
    float clock = ext_clock == 0.0? BASECLOCK_HERTZ : ext_clock;
    float scale = clock / STEPS / freq - 1 + 0.5; // 0.5 is to round later.
    uint8_t prescale = MIN_PRESCALE;

    if (scale > MAX_PRESCALE) {
      prescale = MAX_PRESCALE;
    } else if (scale < MIN_PRESCALE) {
      prescale = MIN_PRESCALE;
    } else {
      prescale = (uint8_t) scale;
    }

    // Setting prescale register or external clock bit when asleep.
    uint8_t mode1 = read(PCA9685PWM::MODE1);
    boolean asleep = (mode1 & PCA9685PWM::MODE1_SLEEP);
    
    if (!asleep) {
      mode1 |= PCA9685PWM::MODE1_SLEEP;
      write(PCA9685PWM::MODE1, mode1);
      delay(5);
    }
    write(PRE_SCALE, prescale);

    // if necessary wake up.
    if (!asleep) {
      mode1 &= (~PCA9685PWM::MODE1_SLEEP);
    }
    if (ext_clock != 0.0) {
      mode1 |= PCA9685PWM::MODE1_EXTCLK;
    }
    write(PCA9685PWM::MODE1, mode1);
    if (asleep) {
      delay(5);
    }
  }
}

float PCA9685PWM::calc_freq(float ext_clock) {
  uint8_t  prescale = read(PRE_SCALE);
  float clock = ext_clock == 0.0? BASECLOCK_HERTZ : ext_clock;
  return clock / STEPS / (prescale + 1);
}

void PCA9685PWM::totem_pole_outputs(boolean totem) {
  uint8_t mode2 = read(PCA9685PWM::MODE2);
  mode2 = totem?
    mode2 | PCA9685PWM::MODE2_INVRT : mode2 & (~PCA9685PWM::MODE2_OUTDRV);
  write(PCA9685PWM::MODE2, mode2);
}

void PCA9685PWM::invert_outputs(boolean invert) {
  uint8_t mode2 = read(PCA9685PWM::MODE2);
  mode2 = invert?
    mode2 | PCA9685PWM::MODE2_INVRT : mode2 & (~PCA9685PWM::MODE2_INVRT);
  write(PCA9685PWM::MODE2, mode2);
}

void PCA9685PWM::reset(void) {
  PCA9685PWM::reset(_i2cPort);
}

// static
void PCA9685PWM::reset(TwoWire *i2cPort) {
  uint8_t va[] = {0x06};
  write(ADR_RESET, i2cPort, va, sizeof(va));
}

uint8_t PCA9685PWM::pwm_register_access(uint8_t port) {
  if (port < n_of_ports) {
    return LED_REGISTER_START + port * REGISTERS_PER_PORT;
  } else {
    return ALL_LED_ON_L;
  }
}

uint8_t PCA9685PWM::number_of_ports(void) { return n_of_ports; }

String PCA9685PWM::type_name(void) { return "PCA9685"; }

// static
boolean PCA9685PWM::isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort) {
  if ((i2cAddr << 1) == PCA9685PWM::ADR_ALLCALL) {
    return false;
  }
  
  // PCA9685 returns 0b00010001	for MODE1, 0b00000100 for MODE 2
  // by default according to the data sheet.
  uint8_t mode1 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9685PWM::MODE1);
  uint8_t mode2 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9685PWM::MODE2);

  /*
  Serial.print("Address = 0x");
  Serial.print(i2cAddr, HEX);
  Serial.print(" MODE1 = 0b");
  Serial.print(mode1, BIN);
  Serial.print(" MODE2 = 0b");
  Serial.println(mode2, BIN);
  */

  if (!((mode1 & 0b10001111) == 0b00000001 && (mode2) == 0b00000100)) {
    return false;
  }
  
  uint8_t subadr1 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9685PWM::SUBADR1);
  uint8_t subadr2 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9685PWM::SUBADR2);
  uint8_t subadr3 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9685PWM::SUBADR3);
  uint8_t allcalladr = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9685PWM::ALLCALLADR);

  /*
  Serial.print("Address = 0x");
  Serial.print(i2cAddr, HEX);
  Serial.print(" Sub1 = 0x");
  Serial.print(subadr1, HEX);
  Serial.print(" Sub2 = 0x");
  Serial.print(subadr2, HEX);
  Serial.print(" Sub3 = 0x");
  Serial.print(subadr3, HEX);
  Serial.print(" AllCall = 0x");
  Serial.println(allcalladr, HEX);
  */

  return subadr1 == PCA9685PWM::ADR_SUB1 &&
         subadr2 == PCA9685PWM::ADR_SUB2 &&
         subadr3 == PCA9685PWM::ADR_SUB3 &&
         allcalladr == PCA9685PWM::ADR_ALLCALL;
}
