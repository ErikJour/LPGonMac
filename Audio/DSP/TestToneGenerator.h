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
static constexpr auto relaxed = std::memory_order_relaxed;

struct TestTone {
    int frame    = 0;
    float sample = 0.0f;

     void reset()
    {
        frame  = 0;
        sample = 0;
    }

     void generateTestTone(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {
        double j = renderer->startingFrameCount;
        double cycleLength = 44100. / SINE_FREQUENCY;
        frame = 0;
        auto *leftChannel = (Float32 *) ioData->mBuffers[0].mData;
        auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;
        for (frame = 0; frame < inNumberFrames; ++frame) {
            sample                = (Float32) sin(2 * M_PI * (j / cycleLength));
            (leftChannel)[frame]  += sample;
            (rightChannel)[frame] += sample;

            j += 1.0;
            renderer->outputData = *leftChannel;
            if (j > cycleLength) { j -= cycleLength; }
        }
        renderer->startingFrameCount = j;
        float current = renderer->outputData.load(relaxed);
        renderer->outputData.store(current, relaxed);

    }
};





