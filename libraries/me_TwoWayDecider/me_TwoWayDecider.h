#pragma once

class TwoWayDecider {
public:
    virtual bool needToStart(int16_t curValue) = 0;
    virtual bool needToStop(int16_t curValue) = 0;
};

class DeciderValToStart : public TwoWayDecider {
protected:
    int16_t startValue_;
    int16_t stopValue_;
public:
    DeciderValToStart(int16_t startValue, int16_t stopValue) {
        startValue_ = startValue;
        stopValue_ = stopValue;
    };
};

class DeciderHighValToStart : public DeciderValToStart {
public:
    bool needToStart(int16_t curValue) {
        return (curValue >= DeciderValToStart::startValue_);
    }
    bool needToStop(int16_t curValue) {
        return (curValue <= DeciderValToStart::stopValue_);
    }
};

class DeciderLowValToStart : public DeciderValToStart {
public:
    bool needToStart(int16_t curValue) {
        return (curValue <= DeciderValToStart::startValue_);
    }
    bool needToStop(int16_t curValue) {
        return (curValue >= DeciderValToStart::stopValue_);
    }
};
