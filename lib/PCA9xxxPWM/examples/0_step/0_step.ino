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

#include <PCA9955APWM.h>
#define DEFAULT_I2C_ADDR_995x 0x65 // At your preference (of your h/w setting)
//PCA9955APWM pwm(DEFAULT_I2C_ADDR_995x);

#include <PCA9626PWM.h>
#define DEFAULT_I2C_ADDR_962x 0x40 // At your preference (of your h/w setting)
// PCA9626PWM pwm(DEFAULT_I2C_ADDR_962x);

#include <PCA9685PWM.h>
#define DEFAULT_I2C_ADDR_9685 0x40 // At your preference (of your h/w setting)
PCA9685PWM pwm(DEFAULT_I2C_ADDR_9685);

#include <Wire.h>

uint8_t n_of_ports = 0;
boolean verbose = false;

void setup() {
  Serial.begin(115200);
  delay(100);

  if (pwm.begin() == false) {
    Serial.println("Fail. Attempt to reset");
    pwm.reset();
    if (pwm.begin() == false) {
      Serial.println("Device does not appear to be connected. Please check "
                     "wiring. Freezing...");
      while (1)
        ;
    }
  }

  Wire.setClock(400000);
  n_of_ports = pwm.number_of_ports();

  verbose = true;
  if (verbose) {
    Serial.print("Number of ports : ");
    Serial.print(n_of_ports);
    Serial.println("");
  }
}

void test_one_by_one(int i) {
  for (int j = 0; j < n_of_ports; j++) {
    pwm.pwm(j, (j == i % n_of_ports) ? 1.0 : 0);
  }
}

int i = 0;

void loop() {
  test_one_by_one(i++);
  delay(200);
  verbose = false;
}
