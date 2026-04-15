// Event.h
#pragma once

#include "辅助类.h"
#include "Gryo/Gryo.h"

class Gyro {
public:
    Gyro() {
        FGyro::GetInstance()->Init();
    }

    bool bGyroConnect() {
        return FGyro::GetInstance() != nullptr;
    }

    bool isRunning() {
        return FGyro::GetInstance()->GetGyroInfo().enable;
    }

    void update(float x, float y, bool enable = true) {
        if (!enable) {
            FGyro::GetInstance()->Unlock_Event();
            return;
        }
        FGyro::GetInstance()->Send_Event(x, y);
    }

    void stop() {
        FGyro::GetInstance()->Unlock_Event();
    }

    void reset() {
        FGyro::GetInstance()->Unlock_Event();
    }
};
