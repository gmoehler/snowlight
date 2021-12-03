#include "tofTask.h"

// much of the code is taken from VL53LOX interrupt example

#define LED_BUILTIN 2

const byte VL53LOX_InterruptPin = 19;
const byte VL53LOX_ShutdownPin = 18;
volatile byte VL53LOX_State = LOW;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

FixPoint1616_t distanceThreshold = 200;

// smoothing window
const uint8_t smoothWinSize = 9;
uint16_t smoothWin[smoothWinSize];
uint8_t smoothWinIndex = 0;
int smoothWinSum = 0;

// analysis window
uint32_t lastAnalysisTime = 0;
uint32_t analysisWinTime = 500;  // analysis time in millis
uint16_t analysisWinMax = 0;
uint16_t analysisWinMin = INT16_MAX;

// resulting actions
uint32_t longHoldStartTime = 0;
uint32_t longHoldDuration = 2000;  // min time for a long "press" gesture

// smoothing window
// value will be too small for first smoothWinSize vals
uint16_t getSmoothedValue(uint16_t newValue) {
    smoothWinSum =
        smoothWinSum - smoothWin[smoothWinIndex];  // remove oldest val from sum
    smoothWin[smoothWinIndex] = newValue;          // add new val
    smoothWinSum += newValue;
    smoothWinIndex = (smoothWinIndex + 1) % smoothWinSize;

    return smoothWinSum / smoothWinSize;
}

void resetLongHold() {
    longHoldStartTime = 0;  // reset start time
}

void doAnalysis() {
    int diff = analysisWinMax - analysisWinMin;
    uint32_t now = millis();

    if (abs(diff) < 10) {
        // no movement (within 10mm): long hold?
        Serial.printf("Static gesture detected (diff: %d).\n", diff);
        Serial.printf("now: %.1f, start: %.1f, last: %.1f, dur: %.1f\n",
                      now / 1000.0, longHoldStartTime / 1000.0,
                      lastAnalysisTime / 1000.0, longHoldDuration / 1000.0);
        if (longHoldStartTime == 0) {
            longHoldStartTime = lastAnalysisTime;
        }
        if (now - longHoldStartTime > longHoldDuration) {
            Serial.printf("Long press detected.\n");
            sendRawToLed({LIGHT_TOGGLE}, TOF);
            resetLongHold();
        }
    } else {
        Serial.printf("Movement detected by %d.\n", diff);
        uint8_t dimDiff = static_cast<uint8_t>(std::min(abs(diff), 255));
        if (diff > 0) {
            sendRawToLed({LIGHT_BRIGHTEN, dimDiff}, TOF);
        } else {
            sendRawToLed({LIGHT_DIM, dimDiff}, TOF);
        }
        resetLongHold();
    }
}

void resetAnalysis() {
    analysisWinMax = 0;
    analysisWinMin = INT16_MAX;
}

static void tofTask(void* arg) {
    for (;;) {
        if (VL53LOX_State == LOW) {
            uint32_t now = millis();
            LOGD(TOF, "Reading a measurement at (%f)... \n", now / 1000.0);

            VL53L0X_RangingMeasurementData_t measure;

            lox.getRangingMeasurement(&measure, false);

            uint16_t distance = measure.RangeMilliMeter;
            uint16_t distSmoothed = getSmoothedValue(distance);

            // phase failures have incorrect data
            if (measure.RangeStatus != 4) {
                LOGD(TOF, "RangeStatus       : %d", measure.RangeStatus);
                LOGI(TOF, "Distance      (mm): %d", distance);
                LOGI(TOF, "Smoothed Dist (mm): %d", distSmoothed);
            } else {
                LOGW(TOF, "Distance out of range.");
            }

            // update Analysis values
            analysisWinMin = std::min(analysisWinMin, distSmoothed);
            analysisWinMax = std::max(analysisWinMax, distSmoothed);

            // detect first time after range re-enter
            if (now - lastAnalysisTime > 2 * analysisWinTime) {
                // need to set last analyis time - otherwise a long hold gesture
                // is detected instantly
                lastAnalysisTime = now;
            }

            if (now > lastAnalysisTime + analysisWinTime) {
                doAnalysis();
                lastAnalysisTime = now;
                resetAnalysis();  // reset min/max
            }

            lox.clearInterruptMask(false);  // to get triggered again

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

    // if lox.begin failes its because it was a warm boot and the VL53LOX is in
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

    // trigger GPIO when range is lower than low threshold
    // options are:
    // VL53L0X_GPIOFUNCTIONALITY_OFF,
    // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
    // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH,
    // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,
    // VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY
    lox.setGpioConfig(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
                      VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
                      VL53L0X_INTERRUPTPOLARITY_LOW);

    // Set Interrupt Treashholds (high threshold not used for
    // THRESHOLD_CROSSED_LOW)
    lox.setInterruptThresholds(distanceThreshold * 65536.0, 0, true);

    // Enable Continous Measurement Mode
    lox.setDeviceMode(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, true);

    LOGD(TOF, "Starting measurement... ");
    lox.startMeasurement();
}

void tof_start(uint8_t prio) {
    xTaskCreate(tofTask, "tofTask", 4096, NULL, prio, NULL);
}
