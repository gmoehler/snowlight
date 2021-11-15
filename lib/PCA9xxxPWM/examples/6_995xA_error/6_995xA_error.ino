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
PCA9955APWM pwm(DEFAULT_I2C_ADDR_995x);

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

void test_err(uint8_t port) {
  uint8_t err = pwm.errflag(port);
  if (err != PCA995xAPWM::NO_ERROR) {
    Serial.print("Port ");
    Serial.print(port);
    Serial.print(" error = ");
    String errstr;
    if (err & PCA995xAPWM::OVERTEMP) {
      Serial.print(" Overtemp ");
    }
    switch (err & PCA995xAPWM::CIRCUIT_ERROR) {
      case PCA995xAPWM::SHORT_CIRCUIT:
        Serial.print("short circuit");
        break;
     case PCA995xAPWM::OPEN_CIRCUIT:
        Serial.print("open circuit");
        break;
      case PCA995xAPWM::CIRCUIT_ERROR:
        Serial.print("open or short circuit");
        break;
    }
    Serial.println("");
  }
}

void test_one_by_one(int i) {
  for (int j = 0; j < n_of_ports; j++) {
    pwm.pwm(j, (j == i % n_of_ports) ? 0.99 : 0);
  }
}

// Set value of LEDOUT[0:3] register.
// val = 0b00: OFF, 0b01: Full ON (no current control),
// val = 0b10: PWM, 0b11: Group dim/blink.
void ledout_port(uint8_t val) {
  uint8_t init_array[] = {
    PCA9955APWM::LEDOUT0 | PCA9955APWM::MODE1_AIF, 0, 0, 0, 0
  };
  val &= 0b11;
  for (int i = 1; i <= 4; i++) {
    init_array[i] = val << 6 | val << 4 | val << 2 | val;
  }
  pwm.write(init_array, sizeof(init_array));
}

void loop() {
  Serial.println("Turn on condition");
  ledout_port(0b10);
  pwm.current(ALLPORTS, 1.0);
  for (int i = 0; i < n_of_ports; i++) {
    test_one_by_one(i);
    test_err(i);
    delay(100);
  }
  delay(500);
  
  Serial.println("Turn off condition - won't detect an open/short circuit error");
  ledout_port(0);
  for (int i = 0; i < n_of_ports; i++) {
    test_err(i);
    delay(100);
  }
  delay(500);

  Serial.println("Turn on all condition");
  ledout_port(0b10);
  pwm.pwm(ALLPORTS, 1.0); // Turn on (almost) all
  test_err(ALLPORTS); // if any errors found
  delay(500);
  
  Serial.println("Turn on FULL condition");
  ledout_port(0b01);
  for (int i = 0; i < n_of_ports; i++) {
    test_err(i);
    delay(100);
  }
  delay(500);
  
  verbose = false;
}
