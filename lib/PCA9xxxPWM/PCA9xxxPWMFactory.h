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

#ifndef _PCA9XXX_PWM_FACTORY_H_
#define _PCA9XXX_PWM_FACTORY_H_

#include <Arduino.h>
#include <Wire.h>

#include "PCA9xxxPWM.h"

class PCA9xxxPWMFactory {
public:
  /** Constructor of this class
   *
   * @param i2cPort Arduino Wire object (default: Wire)
   */
  PCA9xxxPWMFactory(TwoWire *i2cPort = &Wire);
  ~PCA9xxxPWMFactory();

  /** scan I2C bus, populate buf with PCA9xxx devices found.
   *
   * @param buf         Array to push pointers to devices.
   * @param len         Size of buf.
   * @param i2cAddrFrom Start of scan in 7 bit. Default=0x08
   * @param i2cAddrTo   End of scan in 7 bit. Default = 0x77
   * @returns           Number of devices returned in buf.
   * @note Calling this function twice or more will produce duplicate instances
   * of the devices.
   * @note I did not use vector as it's deviated from std::vector in Arduino.
   * @note For default I2C address range see https://www.i2c-bus.org/addressing/
   */
  uint8_t scanDevice(PCA9xxxPWM **buf, uint8_t len, uint8_t i2cAddrFrom = 0x08,
                     uint8_t i2cAddrTo = 0x77);

private:
  TwoWire *_i2cPort;
};

#endif
