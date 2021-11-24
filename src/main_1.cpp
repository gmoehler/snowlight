#include <Arduino.h>
#include <PCA9622PWM.h>

#define DEFAULT_I2C_ADDR_9622 0x70  // At your preference (of your h/w setting)
#define OE_PORT 2                   // port for PCA9622 output enablement

PCA9622PWM pwm(DEFAULT_I2C_ADDR_9622);

#include <Wire.h>

uint8_t n_of_ports = 0;
boolean verbose = true;
float* data;

void setup2() {
    Serial.begin(115200);
    delay(1000);

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
    data = (float*)malloc(n_of_ports * sizeof(float*));

    verbose = true;
    if (verbose) {
        Serial.print("Number of ports : ");
        Serial.print(n_of_ports);
        Serial.println("");
    }
}

void test_one_by_one(int i) {
    for (int j = 0; j < n_of_ports; j++) {
        pwm.pwm(j, (j == i % n_of_ports) ? 0.5 : 0);
    }
}

void test_gradient(boolean exp_on) {
    float step = 1.0 / n_of_ports;
    pwm.exponential_adjustment(exp_on);
    for (int j = 1; j <= n_of_ports; j++) {
        float ratio = step * j;
        pwm.pwm(j - 1, ratio);
        if (verbose) {
            Serial.print("j= ");
            Serial.print(j);
            Serial.print(" ratio= ");
            Serial.println(ratio);
        }
    }
}

void test_all_gradient(boolean exp_on) {
    uint8_t num_steps = 100;
    float step = 1.0 / num_steps;
    pwm.exponential_adjustment(exp_on);
    for (int j = 1; j <= num_steps; j++) {
        float ratio = step * j;
        if (verbose) {
            Serial.print("j= ");
            Serial.print(j);
            Serial.print(" ratio= ");
            Serial.println(ratio);
        }
        for (int k = 0; k < n_of_ports; k++) {
            delay(200);
            pwm.pwm(k, ratio);
        }
        delay(200);
    }
}

void test_all_gradient2(boolean exp_on) {
    uint8_t num_steps = 100;
    float step = 1.0 / num_steps;
    pwm.exponential_adjustment(exp_on);
    for (int j = 1; j <= num_steps; j++) {
        float ratio = step * j;
        if (verbose) {
            Serial.print("j= ");
            Serial.print(j);
            Serial.print(" ratio= ");
            Serial.println(ratio);
        }
        for (int k = 0; k < n_of_ports; k++) {
            data[k] = ratio;
            pwm.pwm(data);
        }
        delay(100);
    }
}

// int i = 0;

void loop2() {
    // test_one_by_one(i++);
    // test_gradient(true);
    test_all_gradient2(true);
    delay(200);
    verbose = false;
}
