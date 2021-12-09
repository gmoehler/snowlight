#include "gestures.h"

WindowSmoother::WindowSmoother() { reset(); }

uint16_t WindowSmoother::smooth(uint16_t newValue) {
    _winSum = _winSum - _win[_winIndex];     // remove oldest val from sum
    _win[_winIndex] = newValue;              // add new val to win
    _winSum += newValue;                     // calc new sum
    _winIndex = (_winIndex + 1) % _winSize;  // advance index

    if (_winIndex == _winSize - 1) {
        winFilledUp = true;  // window is now fully filled
    }

    // use full window once it is completely filled up
    uint8_t curWinSize = winFilledUp ? _winSize : _winIndex;

    return _winSum / curWinSize;  // smoothed value
}

void WindowSmoother::reset() {
    for (int i = 0; i < _winSize; i++) {
        _win[i] = 0;
    }
    _winIndex = 0;
    _winSum = 0;
    winFilledUp = false;
}

AnalysisWindow::AnalysisWindow() { reset(); }

void AnalysisWindow::addValue(uint16_t val, millis_t time) {
    _min = std::min(_min, val);
    _max = std::max(_max, val);

    // first value
    if (!_hasData) {
        _first = val;
        _startTime = time;
        _hasData = true;
    }

    // last value
    _last = val;
    _endTime = time;
}

millis_t AnalysisWindow::getInterval() { return _endTime - _startTime; }

int AnalysisWindow::getValueRange() { return _max - _min; }

int AnalysisWindow::getValueDiff() { return _last - _first; }

uint8_t AnalysisWindow::getAbsValueDiff() {
    int diff = getValueDiff();
    return abs(std::min(UINT8_MAX, diff));
}

bool AnalysisWindow::isUp() { return getValueDiff() > 0; }

void AnalysisWindow::reset() {
    _startTime = 0;
    _endTime = 0;
    _first = 0;
    _last = 0;
    _min = UINT16_MAX;
    _max = 0;
    _hasData = false;
}

GestureRecognition::GestureRecognition() {}

void GestureRecognition::_resetSession() {
    _winsmoother.reset();
    _anaWin.reset();
    _skipCounter = 2;
    _numGesturesInSession = 0;
    _longHoldInSession = false;
}

// returns gesture for current analysis window
// NONE gesture means that analysis will have to go on
// allow only one long hold per session
Gesture GestureRecognition::_detectGesture() {
    if (_anaWin.getInterval() > _longHoldInterval) {
        // after long hold interval we can detect any gesture (incl long hold)
        if (_anaWin.getValueRange() > _stableRange) {
            _numGesturesInSession++;
            return _anaWin.isUp()
                       ? (struct Gesture){MOVE_UP, _anaWin.getAbsValueDiff()}
                       : (struct Gesture){MOVE_DOWN, _anaWin.getAbsValueDiff()};
        }
        // long hold must be almost the first one in the session
        if (!_longHoldInSession && _numGesturesInSession < 5) {
            _numGesturesInSession++;
            _longHoldInSession = true;
            return {LONG_HOLD};
        }
        LOGI(GEST, "Skipping long-hold");

    } else if (_anaWin.getInterval() > _analysisInterval) {
        // after shorter analysis interval we only accept move
        if (_anaWin.getValueRange() > _stableRange) {
            _numGesturesInSession++;
            return _anaWin.isUp()
                       ? (struct Gesture){MOVE_UP, _anaWin.getAbsValueDiff()}
                       : (struct Gesture){MOVE_DOWN, _anaWin.getAbsValueDiff()};
        }
    }
    return {NONE};
}

void GestureRecognition::addTofValue(uint16_t val) {
    millis_t now = millis();

    // after re-entry reset everything
    if (now - _lastValueTime > _gapInterval) {
        LOGI(GEST, "Gap detected");
        _resetSession();
    }
    _lastValueTime = now;

    // skip first n values because they tend to be too high
    if (_skipCounter > 0) {
        LOGI(GEST, "Skipping value");
        _skipCounter--;
        return;
    }

    uint16_t smoothedVal = _winsmoother.smooth(val);
    LOGD(GEST, "Val: %d, Smoothed: %d", val, smoothedVal);

    _anaWin.addValue(smoothedVal, now);

    _lastGesture = _detectGesture();
    if (_lastGesture.type != NONE) {
        LOGI(GEST, "Gesture detected: %s", getGestureAsString().c_str());
        _anaWin.reset();
    }
}

Gesture GestureRecognition::getGesture() { return _lastGesture; }

String GestureRecognition::getGestureAsString() {
    String str = GestureString[_lastGesture.type] + " " + _lastGesture.param;
    return str;
}