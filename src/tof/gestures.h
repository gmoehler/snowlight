#ifndef GESTURES_H
#define GESTURES_H

#include "snowlight.h"

class WindowSmoother {
   public:
    WindowSmoother();
    uint16_t smooth(uint16_t newValue);
    void reset();

   private:
    static const uint8_t _winSize = 9;
    uint16_t _win[_winSize];
    uint8_t _winIndex;
    int _winSum;
    bool winFilledUp;
};

enum GestureType { NONE, LONG_HOLD, MOVE_UP, MOVE_DOWN };
const String GestureString[] = {"NONE", "LONG_HOLD", "MOVE_UP", "MOVE_DOWN"};

struct Gesture {
    GestureType type;
    uint8_t param;
};

class AnalysisWindow {
   public:
    AnalysisWindow();
    void addValue(uint16_t val, millis_t time);
    millis_t getInterval();
    int getValueRange();        // between min and max
    int getValueDiff();         // betweem first and last
    uint8_t getAbsValueDiff();  // uint8_t casted value diff
    bool isUp();
    void reset();

   private:
    millis_t _startTime;  // time of first val added
    millis_t _endTime;    // time of last val added
    uint16_t _first;      // first value in analysis window
    uint16_t _last;       // last value up to this point
    uint16_t _min;        // max value
    uint16_t _max;        // min value
    bool _hasData;        // any values added yet?
};

class GestureRecognition {
   public:
    GestureRecognition();
    void addTofValue(uint16_t val);
    Gesture getGesture();
    String getGestureAsString();

   private:
    WindowSmoother _winsmoother;
    AnalysisWindow _anaWin;

    // interval between 2 values to detect a new session
    const millis_t _gapInterval = 500;
    const millis_t _analysisInterval = 200;
    const millis_t _longHoldInterval = 1500;
    const uint16_t _stableRange = 20;  // in mm

    Gesture _lastGesture = {NONE};
    millis_t _lastValueTime = 0;
    short _skipCounter = 3;  // skip first n values

    Gesture _detectGesture();
    void _resetSession();
};

#endif