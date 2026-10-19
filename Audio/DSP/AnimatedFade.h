//
// Created by Erik Jourgensen on 10/19/26.
//

#pragma once

inline void applyFadeOut(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {
    int frame = 0;
    float deduction = 0.9f;
    auto *leftChannel = (Float32 *) ioData->mBuffers[0].mData;
    auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;

    for (frame = 0; frame < inNumberFrames; ++frame) {
        (leftChannel)[frame]  *= deduction;
        (rightChannel)[frame] *= deduction;
        deduction *= 0.001;
    }

}
