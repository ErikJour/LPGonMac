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
#define SINE_FREQUENCY 300.0


inline void generateTestTone(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {
    double j = renderer->startingFrameCount;
    double cycleLength = 44100. / SINE_FREQUENCY;
    int frame = 0;
    //Iterate over samples and fill each inside a buffer
    for (frame = 0; frame < inNumberFrames; ++frame) {
        //Fill left channel
        auto *data = (Float32 *) ioData->mBuffers[0].mData;
        (data)[frame] = (Float32) sin(2 * M_PI * (j / cycleLength));
        //Fill right channel
        data = (Float32 *) ioData->mBuffers[1].mData;
        (data)[frame] = (Float32) sin(2 * M_PI * (j / cycleLength));

        j += 1.0;
        renderer->outputData = *data;
        //Correctly prints current value
        if (j > cycleLength) { j -= cycleLength; }
    }
    renderer->startingFrameCount = j;
    // Atomic write compatible with both C and C++
    float current = renderer->outputData.load(std::memory_order_relaxed);
    renderer->outputData.store(current, std::memory_order_relaxed);


}
