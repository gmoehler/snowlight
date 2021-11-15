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
  PCA9xxxPWMFactory - Arduino / C++ class for NXP PCA9xxx I2C connected PWM
  devices.

  Make Asayake to Wake Project.
  Kiyo Chinzei
  https://github.com/kchinzei/PCA9xxxPWM
*/

#include "PCA9xxxPWMFactory.h"
#include "PCA9xxxPWM.h"

#include "PCA9626PWM.h"
#include "PCA9685PWM.h"
#include "PCA9955APWM.h"

PCA9xxxPWMFactory::PCA9xxxPWMFactory(TwoWire *i2cPort) : _i2cPort(i2cPort) {}

PCA9xxxPWMFactory::~PCA9xxxPWMFactory() {}

uint8_t PCA9xxxPWMFactory::scanDevice(PCA9xxxPWM **buf, uint8_t len,
                                      uint8_t i2cAddrFrom, uint8_t i2cAddrTo) {
  uint8_t iFound = 0;

  _i2cPort->begin();

  // There may be substantial side effects to reset ALL devices.
  PCA9xxxPWM::reset(_i2cPort);
  
  for (uint8_t i = i2cAddrFrom; i <= i2cAddrTo; i++) {
    if (i > 0x7F) {
      break;
    }
    if (iFound == len) {
      break;
    }

    // When you add a concrete class of PCA9xxxPWM, add here.

    if (PCA9685PWM::isMyDevice(i, _i2cPort)) {
      buf[iFound++] = new PCA9685PWM(i, _i2cPort);
      Serial.print("Find 9685 adr = ");
      Serial.println(i, HEX);
    } else if (PCA9626PWM::isMyDevice(i, _i2cPort)) {
      buf[iFound++] = new PCA9626PWM(i, _i2cPort);
      Serial.print("Find 962x adr = ");
      Serial.println(i, HEX);
    } else if (PCA9955APWM::isMyDevice(i, _i2cPort)) {
      buf[iFound++] = new PCA9955APWM(i, _i2cPort);
      Serial.print("Find 995x adr = ");
      Serial.println(i, HEX);
    }
  }
  return iFound;
}
