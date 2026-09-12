//
// Created by 王 on 2025/12/7.
//
#include "VectorStruct.h"
#ifndef ANDROID_PHYSX_READGAME_H
#define ANDROID_PHYSX_READGAME_H


namespace ReadGame {

    void ReadGameData();
    void Initialization();
    void Raycast();
};

bool CustomLineTrace(const Vector3& location, const Vector3& coord);

#endif //ANDROID_PHYSX_READGAME_H
