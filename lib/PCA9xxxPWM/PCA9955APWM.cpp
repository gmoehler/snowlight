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

#include "PCA9955APWM.h"

PCA9955APWM::PCA9955APWM(uint8_t i2cAddr, TwoWire *i2cPort)
    : PCA995xAPWM(i2cAddr, i2cPort), n_of_ports(16) {}

PCA9955APWM::~PCA9955APWM() {}

void PCA9955APWM::init(void) {
  uint8_t init_array[] = {
      PCA995xAPWM::AUTO_INCREMENT | REGISTER_START, //  Command
      0x00,
      0x00, //  MODE1, MODE2
      0xAA,
      0xAA,
      0xAA,
      0xAA, //  LEDOUT[3:0] to PWM mode
      0x80,
      0x00, //  GRPPWM, GRPFREQ
  };

  write(init_array, sizeof(init_array));
  pwm(ALLPORTS, 0.0);
  current(ALLPORTS, 1.0);
  // gradation_group_clear();
}

void PCA9955APWM::exponential_adjustment(boolean exp_on) {
  uint8_t exp_en = 1 << 2;
  uint8_t mode2 = read(MODE2);

  // FIXME: smarter way??
  mode2 = exp_on ? mode2 | exp_en : mode2 & (~exp_en);

  write(MODE2, mode2);
}

float PCA9955APWM::simple_exp(float refIn) {
  // Serial.println("override simple_exp()");
  return refIn;
}

uint8_t PCA9955APWM::pwm_register_access(uint8_t port) {
  if (port < n_of_ports) {
    return PWM_REGISTER_START + port;
  } else {
    return PWMALL;
  }
}

uint8_t PCA9955APWM::current_register_access(uint8_t port) {
  if (port < n_of_ports) {
    return IREF_REGISTER_START + port;
  } else {
    return IREFALL;
  }
}

uint8_t PCA9955APWM::errflag(uint8_t port) {
  auto err_bit = [](uint8_t port, uint8_t err_byte) {
    uint8_t shiftbits = (port % 4)*2;
    return (err_byte >> shiftbits) & 0b11;
  };
  uint8_t n_of_ports = number_of_ports();
  uint8_t ret = NO_ERROR;

  // 1. Get error flags.
  uint8_t mode2 = read(MODE2);

  // 2. Open/Short circuit?
  if (mode2 & MODE2_ERROR) {
    if (port < n_of_ports) {
      uint8_t reg_addr = ERR_REGISTER_START + port/4;
      uint8_t err_byte = read(reg_addr);
      ret = err_bit(port, err_byte);
    } else if (port == ALLPORTS) {
      ret = CIRCUIT_ERROR;
    }

    // Clear it. It may hide errors in other ports.
    mode2 |= MODE2_CLRERR;
    write(MODE2, mode2);
  }

  // 3. Over temperature?
  if (mode2 & MODE2_OVERTEMP) {
    ret |= OVERTEMP;
  }

  return ret;
}

uint8_t PCA9955APWM::number_of_ports(void) { return n_of_ports; }

String PCA9955APWM::type_name(void) { return "PCA9955A"; }

// static
boolean PCA9955APWM::isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort) {
  return PCA995xAPWM::isMyDevice(i2cAddr, i2cPort) &&
         PCA9955APWM::_isMyDevice(i2cAddr, i2cPort);
}

// static
boolean PCA9955APWM::_isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort) {
  // PCA9955A returns 0b11101100 for SUBADR1 - SUBADR3 by default
  uint8_t subadr1 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9955APWM::SUBADR1);
  uint8_t subadr2 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9955APWM::SUBADR2);
  uint8_t subadr3 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9955APWM::SUBADR3);

  return subadr1 == 0b11101100 && subadr2 == 0b11101100 &&
         subadr3 == 0b11101100;
}
