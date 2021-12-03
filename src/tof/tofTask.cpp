#include "tofTask.h"

// much of the code is taken from VL53LOX interrupt example

#define LED_BUILTIN 2

const byte VL53LOX_InterruptPin = 19;
const byte VL53LOX_ShutdownPin = 18;
volatile byte VL53LOX_State = LOW;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

FixPoint1616_t lowThr = 50;
FixPoint1616_t highThr = 100;

static void tofTask(void* arg) {
    for (;;) {
        if (VL53LOX_State == LOW) {
            VL53L0X_RangingMeasurementData_t measure;
            LOGI(TOF, "Reading a measurement... ");
            // pass in 'true' to get debug data printout!
            lox.getRangingMeasurement(&measure, false);

            // phase failures have incorrect data
            if (measure.RangeStatus != 4) {
                LOGI(TOF, "Distance (mm): %d", measure.RangeMilliMeter);
            } else {
                LOGW(TOF, "Distance out of range.");
            }
            // you have to clear the interrupt to get triggered again
            lox.clearInterruptMask(false);

        } else {
            digitalWrite(LED_BUILTIN, LOW);
            delay(10);
        }
    }
}

void VL53LOXISR() {
    // Read if we are high or low
    VL53LOX_State = digitalRead(VL53LOX_InterruptPin);
    // set the built in LED to reflect in range on for Out of range off for in
    // range
    digitalWrite(LED_BUILTIN, VL53LOX_State);
}

void tof_setup() {
    LOGD(TOF, "Setting up TOF sensor:");

    pinMode(VL53LOX_ShutdownPin, INPUT_PULLUP);
    pinMode(VL53LOX_InterruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(VL53LOX_InterruptPin), VL53LOXISR,
                    CHANGE);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // if lox.begin failes its becasue it was a warm boot and the VL53LOX is in
    // continues mesurement mode we can use an IO pin to reset the device in
    // case we get stuck in this mode
    int numTries = 10;
    int tries = 0;
    while (!lox.begin() && tries < numTries) {
        LOGE(TOF, "Failed to boot VL53L0X");
        LOGI(TOF, "Adafruit VL53L0X XShut set Low to Force HW Reset");
        digitalWrite(VL53LOX_ShutdownPin, LOW);
        delay(100);
        digitalWrite(VL53LOX_ShutdownPin, HIGH);
        LOGI(TOF, "Adafruit VL53L0X XShut set high to Allow Boot");
        delay(100);
        tries++;
    }

    // Second Parameter options are VL53L0X_GPIOFUNCTIONALITY_OFF,
    // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
    // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH,
    // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,
    // VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY

    LOGD(TOF,
         "Set GPIO Config so if range is lower the LowThreshold trigger Gpio "
         "Pin ");
    lox.setGpioConfig(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
                      VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
                      VL53L0X_INTERRUPTPOLARITY_LOW);

    // Set Interrupt Treashholds
    // Low reading set to 50mm  High Set to 100mm
    FixPoint1616_t LowThreashHold = (lowThr * 65536.0);
    FixPoint1616_t HighThreashHold = (highThr * 65536.0);
    LOGD(TOF, "Set Interrupt Threasholds... ");
    lox.setInterruptThresholds(LowThreashHold, HighThreashHold, true);

    // Enable Continous Measurement Mode
    LOGD(TOF, "Set Mode VL53L0X_DEVICEMODE_CONTINUOUS_RANGING... ");
    lox.setDeviceMode(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, true);

    LOGD(TOF, "Starting measurement... ");
    lox.startMeasurement();
}

void tof_start(uint8_t prio) {
    xTaskCreate(tofTask, "tofTask", 4096, NULL, prio, NULL);
}
