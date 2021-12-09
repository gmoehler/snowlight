#include "tofTask.h"

#include "gestures.h"

// much of the code is taken from VL53LOX interrupt example

#define LED_BUILTIN 2

const byte VL53LOX_InterruptPin = 19;
const byte VL53LOX_ShutdownPin = 18;
volatile byte VL53LOX_State = LOW;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure;

GestureRecognition gestureRecognition = GestureRecognition();

FixPoint1616_t distanceThreshold = 300;  // in mm

static void tofTask(void* arg) {
    for (;;) {
        if (VL53LOX_State == LOW) {
            LOGD(TOF, "Reading a measurement");

            lox.getRangingMeasurement(&measure, false);
            uint16_t distance = measure.RangeMilliMeter;

            // phase failures have incorrect data
            if (measure.RangeStatus != 4) {
                LOGD(TOF, "RangeStatus       : %d", measure.RangeStatus);
                LOGI(TOF, "Distance      (mm): %d", distance);
            } else {
                LOGW(TOF, "Distance out of range.");
            }

            gestureRecognition.addTofValue(distance);
            Gesture gesture = gestureRecognition.getGesture();
            if (gesture.type != NONE) {
                LOGD(TOF, "Gesture detected: %s",
                     gestureRecognition.getGestureAsString().c_str());
            }
            switch (gesture.type) {
                case LONG_HOLD:
                    sendRawToLed({LIGHT_TOGGLE}, TOF);
                    break;
                case MOVE_UP:
                    sendRawToLed({LIGHT_BRIGHTEN, gesture.param}, TOF);
                    break;
                case MOVE_DOWN:
                    sendRawToLed({LIGHT_DIM, gesture.param}, TOF);
                    break;
                case NONE:
                default:
                    break;  // do nothing
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
