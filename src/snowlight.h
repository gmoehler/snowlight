#ifndef SNOWLIGHT_H
#define SNOWLIGHT_H

// default compile time logging level need to be defined
#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
//#define CONFIG_LOG_COLORS 1 // does not work in VSC terminal

#include <Arduino.h>

#include "esp_log.h"

// need to define my own logging macros since other are not activated by
// LOG_LOCAL_LEVEL as expected
#ifndef LOG_MACROS
#define LOG_MACROS
#define LOGE(tag, format, ...)                                   \
    if (LOG_LOCAL_LEVEL >= ESP_LOG_ERROR) {                      \
        esp_log_write(ESP_LOG_ERROR, tag, LOG_FORMAT(E, format), \
                      esp_log_timestamp(), tag, ##__VA_ARGS__);  \
    }
#define LOGW(tag, format, ...)                                  \
    if (LOG_LOCAL_LEVEL >= ESP_LOG_WARN) {                      \
        esp_log_write(ESP_LOG_WARN, tag, LOG_FORMAT(W, format), \
                      esp_log_timestamp(), tag, ##__VA_ARGS__); \
    }
#define LOGI(tag, format, ...)                                  \
    if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO) {                      \
        esp_log_write(ESP_LOG_INFO, tag, LOG_FORMAT(I, format), \
                      esp_log_timestamp(), tag, ##__VA_ARGS__); \
    }
#define LOGD(tag, format, ...)                                   \
    if (LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG) {                      \
        esp_log_write(ESP_LOG_DEBUG, tag, LOG_FORMAT(D, format), \
                      esp_log_timestamp(), tag, ##__VA_ARGS__);  \
    }
#define LOGV(tag, format, ...)                                     \
    if (LOG_LOCAL_LEVEL >= ESP_LOG_VERBOSE) {                      \
        esp_log_write(ESP_LOG_VERBOSE, tag, LOG_FORMAT(V, format), \
                      esp_log_timestamp(), tag, ##__VA_ARGS__);    \
    }
#endif

#define N_CMD_FIELDS 1  // start with only command without args

// serial baud rate
//#define UART_BAUD 115200
#define UART_BAUD 921600

enum Color {
    WHITE,      // 0
    BLACK,      // 1
    RED,        // 2
    GREEN,      // 3
    BLUE,       // 4
    YELLOW,     // 5
    LILA,       // 6
    CYAN,       // 7
    ORANGE,     // 8
    RAINBOW,    // 9
    PALE_WHITE  // 10
};

// setup file tags for logger
#define MAIN "MAIN"      // main program task
#define LIGHTCMD "LCMD"  // light task

#define LEDS "LEDS"     // led output task
#define STEPPER "STEP"  // stepper output task
#define BUTTON "BUTT"   // button task

#endif
