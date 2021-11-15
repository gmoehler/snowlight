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

#include "PCA9xxxPWMFactory.h"

#include <Wire.h>
#define MAXDEVICE 4

PCA9xxxPWM *pwms[MAXDEVICE];
uint8_t ndevice = 0;

boolean verbose = true;

void setup() {
  Serial.begin(115200);
  delay(5000);

  PCA9xxxPWMFactory factory;
  ndevice = factory.scanDevice(pwms, MAXDEVICE);

  Serial.print("Number of device found= ");
  Serial.println(ndevice);

  for (int i = 0; i < ndevice; i++) {
    PCA9xxxPWM *pwm = pwms[i];

    if (pwm->begin() == false) {
        Serial.println("Fail. Attempt to reset");
        pwm->reset();
        if (pwm->begin() == false) {
            Serial.print("Device ");
            Serial.print(i);
            Serial.println("does not appear to respond. Freezing...");
            while (1)
                ;
        }
    }
    if (verbose) {
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(pwm->type_name());
      Serial.print(" Number of ports : ");
      Serial.print(pwm->number_of_ports());
      Serial.println("");
    }
  }

  Wire.setClock(200000);
}

void test_one_by_one(PCA9xxxPWM *pwm, int i) {
  int n_of_ports = pwm->number_of_ports();
  for (int j = 0; j < n_of_ports; j++) {
    pwm->pwm(j, (j == i % n_of_ports) ? 1.0 : 0);
  }
}

int ii = 0;

void loop() {
  for (int i = 0; i < ndevice; i++) {
    test_one_by_one(pwms[i], ii);
  }
  ii++;
  delay(200);
  verbose = false;
}
