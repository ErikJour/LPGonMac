//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "../AudioUtilities.h"
#include "../AudioTypes.h"
#define SINE_FREQUENCY 880.0


inline void generateTestTone(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames)
{
    double j                 = renderer->startingFrameCount;
    double cycleLength       = 44100. / SINE_FREQUENCY;
    int frame                = 0;
    //Iterate over samples and fill each inside a buffer
    for (frame = 0; frame < inNumberFrames; ++frame)
    {
        //Fill left channel
        Float32 *data = (Float32*)ioData->mBuffers[0].mData;
        (data)[frame] = (Float32)sin (2 * M_PI * (j / cycleLength));
        //Fill right channel
        data          = (Float32*)ioData->mBuffers[1].mData;
        (data)[frame] = (Float32)sin (2 * M_PI * (j / cycleLength));

        j += 1.0;
        if (j > cycleLength) { j -= cycleLength; }
    }
    renderer->startingFrameCount = j;
}
