//
// Created by Erik Jourgensen on 10/7/26.
//
#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "../AudioUtilities.h"
#include "../AudioTypes.h"
#define SINE_FREQUENCY 880.0

float gain = 0.75f;


inline void processGain(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {
    int frame = 0;
    //Iterate over samples and fill each inside a buffer
    for (frame = 0; frame < inNumberFrames; ++frame) {
        //Fill left channel
        auto *data = (Float32 *) ioData->mBuffers[0].mData;
        (data)[frame] *= gain;
        //Fill right channel
        data = (Float32 *) ioData->mBuffers[1].mData;
        (data)[frame] *= gain;
    }

}
