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

#include "PCA995xAPWM.h"

PCA995xAPWM::PCA995xAPWM(uint8_t i2cAddr, TwoWire *i2cPort)
    : PCA9xxxPWM(i2cAddr, i2cPort) {}

PCA995xAPWM::~PCA995xAPWM() {}

boolean PCA995xAPWM::begin() {
  if (PCA9xxxPWM::isConnected(_i2cAddr, _i2cPort) &&
      PCA995xAPWM::isMyDevice(_i2cAddr, _i2cPort)) {
    return PCA9xxxPWM::begin();
  } else {
    return false;
  }
}

void PCA995xAPWM::reset(void) {
  PCA995xAPWM::reset(_i2cPort);
}

// static
void PCA995xAPWM::reset(TwoWire *i2cPort) {
  uint8_t va[] = {0x06};
  write(ADR_RESET, i2cPort, va, sizeof(va));
}

void PCA995xAPWM::current(uint8_t port, float v) {
  uint8_t reg_addr = current_register_access(port);
  write(reg_addr, (uint8_t)(v * 255.0));
}

void PCA995xAPWM::current(float *vp) {
  uint8_t n_of_ports = number_of_ports();
  uint8_t data[n_of_ports + 1];

  *data = current_register_access(0) | AUTO_INCREMENT;

  for (int i = 1; i <= n_of_ports; i++) {
    data[i] = (uint8_t)(*vp++ * 255.0);
  }
  write(data, sizeof(data));
}

// static
boolean PCA995xAPWM::isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort) {
  if (i2cAddr == ADR_ALLCALL || i2cAddr == ADR_SUBADR_DEFAULT)
    return false;
  // PCA995x devices commonly return 0b10001001 for MODE1, 0b00000*01 for MODE 2
  // by default. Bit 2 of MODE2 differs between 9955 and 9956.
  uint8_t mode1 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9xxxPWM::MODE1);
  uint8_t mode2 = PCA9xxxPWM::read(i2cAddr, i2cPort, PCA9xxxPWM::MODE2);

  /*
  Serial.print(" MODE1 = ");
  Serial.print(mode1, BIN);
  Serial.print(" MODE2 = ");
  Serial.println(mode2, BIN);
  */

  // However, experiments reveal that
  // 1) MODE1 bit 7 (auto increment) can be 0,
  // 2) MODE1 bit 4 (sleep) can be 0,
  // 3) MODE1 bit 0 (allcall) can be 0,
  // 4) MODE2 bit 2 (exponential) can be 0.
  // return mode1 == 0b00001001 && (mode2 | 1 << 2) == 0b00000101;
  return (mode1 & 0b01110110) == 0b00000000 &&
         (mode2 & 0b11111011) == 0b00000001;
}
