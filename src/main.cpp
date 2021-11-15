#include <Arduino.h>
#include <PCA9622PWM.h>

#define DEFAULT_I2C_ADDR_962x 0x40  // At your preference (of your h/w setting)
#define OE_PORT 9                   // port for PCA9622 output enablement

PCA9622PWM pwm(DEFAULT_I2C_ADDR_962x, OE_PORT);

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
            Serial.println(
                "Device does not appear to be connected. Please check "
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
