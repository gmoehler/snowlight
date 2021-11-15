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

#include <PCA9685PWM.h>
#define DEFAULT_I2C_ADDR_9685 0x42 // At your preference (of your h/w setting)
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
  if (pwm.type_name() == "PCA9685") {
    pwm.freq(1000);
    pwm.invert_outputs(true);
    pwm.totem_pole_outputs(false);
    Serial.print("Frequency: ");
    Serial.println(pwm.calc_freq(), 2);
  }

  // Wire.setClock(400000);
  n_of_ports = pwm.number_of_ports();

  verbose = true;
  if (verbose) {
    Serial.print("Number of ports : ");
    Serial.print(n_of_ports);
    Serial.println("");

    print_mode_register();
    for (int j=0; j< n_of_ports; j++)
      print_raw_led_register(j);
  }
}

void print_mode_register() {
  uint8_t m1 = pwm.read(PCA9685PWM::MODE1);
  uint8_t m2 = pwm.read(PCA9685PWM::MODE2);
  Serial.print(" MODE1 Restart=");
  Serial.print(m1&PCA9685PWM::MODE1_RESTART? 1:0);
  Serial.print(" ExtClk=");
  Serial.print(m1&PCA9685PWM::MODE1_EXTCLK? 1:0);
  Serial.print(" AutoIncr=");
  Serial.print(m1&PCA9685PWM::MODE1_AI? 1:0);
  Serial.print(" Sleep=");
  Serial.print(m1&PCA9685PWM::MODE1_SLEEP? 1:0);
  Serial.print(" Sub1=");
  Serial.print(m1&PCA9685PWM::MODE1_SUB1? 1:0);
  Serial.print(" Sub2=");
  Serial.print(m1&PCA9685PWM::MODE1_SUB2? 1:0);
  Serial.print(" Sub3=");
  Serial.print(m1&PCA9685PWM::MODE1_SUB3? 1:0);
  Serial.print(" AllCall=");
  Serial.println(m1&PCA9685PWM::MODE1_ALLCALL? 1:0);

  Serial.print(" MODE2 Invrt=");
  Serial.print(m2&PCA9685PWM::MODE2_INVRT? 1:0);
  Serial.print(" OCH=");
  Serial.print(m2&PCA9685PWM::MODE2_OCH? 1:0);
  Serial.print(" OutDrv=");
  Serial.print(m2&PCA9685PWM::MODE2_OUTDRV? "Totem":"Open-drain");
  Serial.print(" OutNE=");
  Serial.println(m2&0x03, BIN);
}
    
void print_raw_led_register(uint8_t port) {
  uint8_t reg = PCA9685PWM::LED_REGISTER_START + 4*port;
  uint16_t on = pwm.read(reg++);
  uint8_t hon = pwm.read(reg++);
  on += (hon & 0x0F) << 8;
  uint16_t off = pwm.read(reg++);
  uint8_t hoff = pwm.read(reg);
  off += (hoff & 0x0F) << 8;
  Serial.print("Port [");
  Serial.print(port);
  Serial.print("] On= ");
  Serial.print(on);
  Serial.print(" Off= ");
  Serial.print(off);
  Serial.print(" Full On= ");
  Serial.print(hon& 0x10, HEX);
  Serial.print(" Full Off= ");
  Serial.println(hoff& 0x10, HEX);
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
