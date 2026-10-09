//
// Created by Erik Jourgensen on 10/9/26.
//

#pragma once

inline void applyGain(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames, float gain) {
    int frame = 0;
    auto *leftChannel = (Float32 *) ioData->mBuffers[0].mData;
    auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;

    for (frame = 0; frame < inNumberFrames; ++frame) {
        (leftChannel)[frame]  *= gain;
        (rightChannel)[frame] *= gain;
    }

}
