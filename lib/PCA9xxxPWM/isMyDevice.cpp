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
#include "PCA9685PWM.h"
#include "PCA995xAPWM.h"
#include "PCA9xxxPWM.h"

// static
boolean PCA9xxxPWM::isMyDevice(uint8_t i2cAddr, TwoWire *wirePort) {
  // If you add a subclass of me, call isMyDevice() of that class
  if (PCA9685PWM::isMyDevice(i2cAddr, wirePort)) {
    return true;
  }

  if (PCA962xPWM::isMyDevice(i2cAddr, wirePort)) {
    return true;
  }

  if (PCA995xAPWM::isMyDevice(i2cAddr, wirePort)) {
    return true;
  }

  return false;
}

// static
void PCA9xxxPWM::reset(TwoWire *i2cPort) {
  PCA962xPWM::reset(i2cPort);
  PCA9685PWM::reset(i2cPort);
  PCA995xAPWM::reset(i2cPort);
}
