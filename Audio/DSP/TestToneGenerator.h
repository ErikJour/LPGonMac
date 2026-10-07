//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "../AudioUtilities.h"
#include "../AudioTypes.h"
#include <iostream>
#include <stdatomic.h>
#include <cmath>
#define SINE_FREQUENCY 880.0


inline void generateTestTone(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {
    double j = renderer->startingFrameCount;
    double cycleLength = 44100. / SINE_FREQUENCY;
    int frame = 0;
    auto *leftData  = (Float32 *) ioData->mBuffers[0].mData;
    auto *rightData = (Float32 *) ioData->mBuffers[1].mData;

    for (frame = 0; frame < inNumberFrames; ++frame) {
        auto sample = (Float32) sin(2 * M_PI * (j / cycleLength));

        (leftData)[frame]  = sample;
        (rightData)[frame] = sample;

        j += 1.0;

        renderer->outputData = *leftData;

        if (j > cycleLength) { j -= cycleLength; }
    }
    renderer->startingFrameCount = j;

    float current = renderer->outputData.load(std::memory_order_relaxed);
    renderer->outputData.store(current, std::memory_order_relaxed);

}
