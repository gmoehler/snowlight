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

#include "PCA9xxxPWM.h"

PCA9xxxPWM::PCA9xxxPWM(uint8_t i2cAddr, TwoWire *i2cPort)
    : _i2cAddr(i2cAddr), _i2cPort(i2cPort), use_exponential(false) {}

PCA9xxxPWM::~PCA9xxxPWM() {}

boolean PCA9xxxPWM::begin() {
    _i2cPort->begin();
    init();  // in the concrete class.
    return true;
}

boolean PCA9xxxPWM::isConnected() {
    if (!_i2cPort) {
        return false;  // You call it before begin().
    }

    return PCA9xxxPWM::isConnected(_i2cAddr, _i2cPort);
}

// static
boolean PCA9xxxPWM::isConnected(uint8_t i2cAddr, TwoWire *i2cPort) {
    i2cPort->begin();
    i2cPort->beginTransmission(i2cAddr);
    return (i2cPort->endTransmission() == 0);
}

void PCA9xxxPWM::exponential_adjustment(boolean exp_on) {
    use_exponential = exp_on;
}

/*
  Rough approximation of exponential scale between 0-255.
  See Fig 5. "Linear and exponential adjustment curves" of
  PCA9955B Product Data Sheet
  https://www.nxp.com/docs/en/data-sheet/PCA9955B.pdf
  It will be overridden in PCA9955APWM class because this
  device has its own function.
 */
float PCA9xxxPWM::simple_exp(float refRawIn) {
    float refIn = fmin(fmax(refRawIn, 0.0), 1.0);
    float refOut = refIn;

    if (use_exponential) {
        if (0 <= refIn && refIn < 64.0 / 255) {
            refOut = 6.0 / 255 * refIn / 0.25;
        } else if (refIn < 128.0 / 255) {
            refOut = 12.0 / 255 * (refIn - 64.0 / 255) / 0.25 + 6.0 / 255;
        } else if (refIn < 192.0 / 255) {
            refOut = 56.0 / 255 * (refIn - 128.0 / 255) / 0.25 + 18.0 / 255;
        } else if (refIn < 1) {
            refOut = 181.0 / 255 * (refIn - 192.0 / 255) / 0.25 + 74.0 / 255;
        }
    }
    return refOut;
}

uint8_t PCA9xxxPWM::simple_exp2(uint8_t refIn) {
    uint8_t refOut = refIn;

    if (use_exponential) {
        if (refIn < 64) {
            refOut = 6 * refIn / 64;
        } else if (refIn < 128) {
            refOut = 12.0 / 64.0 * (refIn - 64) + 6;
        } else if (refIn < 192) {
            refOut = 56.0 / 64.0 * (refIn - 128) + 18;
        } else {
            refOut = 181.0 / 64.0 * (refIn - 192) + 74;
        }
    }
    return refOut;
}

void PCA9xxxPWM::pwm(uint8_t port, float v) {
    uint8_t reg_addr = pwm_register_access(port);

    v = simple_exp(v);
    if (ALLPORTS == reg_addr) {
        uint8_t np = number_of_ports();
        float va[np];

        for (int i = 0; i < np; i++) {
            va[i] = v;
        }
        pwm(va);
    } else {
        /*
        Serial.print("Writing PWM: ");
        Serial.println(v, BIN);
        */
        write(reg_addr, v * 255);
    }
}

void PCA9xxxPWM::pwm2(uint8_t port, uint8_t v) {
    uint8_t reg_addr = pwm_register_access(port);

    uint8_t val = simple_exp2(v);
    if (ALLPORTS == reg_addr) {
        uint8_t np = number_of_ports();
        uint8_t va[np];

        for (int i = 0; i < np; i++) {
            va[i] = val;
        }
        pwm2(va);
    } else {
        /*
        Serial.print("Writing PWM: ");
        Serial.println(v, BIN);
        */
        write(reg_addr, val);
    }
}

void PCA9xxxPWM::pwm(float *vp) {
    uint8_t n_of_ports = number_of_ports();
    uint8_t data[n_of_ports + 1];

    *data = pwm_register_access(0) | AUTO_INCREMENT_PWM;

    for (int i = 1; i <= n_of_ports; i++) {
        data[i] = simple_exp(*vp++) * 255;
    }

    Serial.print("Writing PWM: ");
    for (int i = 0; i <= n_of_ports; i++) {
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    write(data, sizeof(data));
}

void PCA9xxxPWM::pwm2(uint8_t *vp) {
    uint8_t n_of_ports = number_of_ports();
    uint8_t data[n_of_ports + 1];

    *data = pwm_register_access(0) | AUTO_INCREMENT_PWM;

    for (int i = 1; i <= n_of_ports; i++) {
        data[i] = simple_exp2(*vp++);
    }

    Serial.print("Writing PWM: ");
    for (int i = 0; i <= n_of_ports; i++) {
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    write(data, sizeof(data));
}

boolean PCA9xxxPWM::write(uint8_t reg_addr, uint8_t val) {
    uint8_t data[] = {reg_addr, val};
    return PCA9xxxPWM::write(_i2cAddr, _i2cPort, data, sizeof(data));
}

boolean PCA9xxxPWM::write(uint8_t *val, uint8_t len) {
    return PCA9xxxPWM::write(_i2cAddr, _i2cPort, val, len);
}

// static
boolean PCA9xxxPWM::write(uint8_t addr, TwoWire *i2cPort, uint8_t *val,
                          uint8_t len) {
    i2cPort->beginTransmission(addr);
    i2cPort->write(val, len);
    return (i2cPort->endTransmission() == 0);
}

uint8_t PCA9xxxPWM::read(uint8_t reg_addr) {
    return PCA9xxxPWM::read(_i2cAddr, _i2cPort, reg_addr);
}

// static
uint8_t PCA9xxxPWM::read(uint8_t i2c_addr, TwoWire *i2cPort, uint8_t reg_addr) {
    i2cPort->beginTransmission(i2c_addr);
    i2cPort->write(reg_addr);
    if (i2cPort->endTransmission() != 0) {
        return 0;  // Device failed to ack
    }

    i2cPort->requestFrom(i2c_addr, (uint8_t)1);
    if (i2cPort->available()) {
        return (i2cPort->read());
    }

    return 0;  // Device failed to respond
}
