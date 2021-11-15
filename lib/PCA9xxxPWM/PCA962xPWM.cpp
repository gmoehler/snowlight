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

#include "PCA962xPWM.h"

PCA962xPWM::PCA962xPWM(uint8_t i2cAddr, TwoWire *i2cPort)
    : PCA9xxxPWM(i2cAddr, i2cPort) {}

PCA962xPWM::~PCA962xPWM() {}

boolean PCA962xPWM::begin() {
  if (PCA9xxxPWM::isConnected(_i2cAddr, _i2cPort) &&
      PCA962xPWM::isMyDevice(_i2cAddr, _i2cPort)) {
    return PCA9xxxPWM::begin();
  } else {
    return false;
  }
}

void PCA962xPWM::reset(void) {
  PCA962xPWM::reset(_i2cPort);
}

// static
void PCA962xPWM::reset(TwoWire *i2cPort) {
  uint8_t va[] = {0xA5, 0x5A};
  write(ADR_RESET, i2cPort, va, sizeof(va));
}

// static
boolean PCA962xPWM::isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort) {
  if (i2cAddr == ADR_ALLCALL)
    return false;

  // PCA962x devices commonly returns 0b10010001	for MODE1, 0b00000101
  // for MODE 2 by default according to the data sheet.
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
  // 2) MODE1 bit 4 (sleep) can be 0.
  return (mode1 & 0b01101111) == 0b00000001 && mode2 == 0b00000101;
}
