//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <atomic>
#include "DSP/AnimatedFFT.h"

struct MyRenderer
{
    AudioUnit          outputUnit         { nullptr };
    double             startingFrameCount { 0.0 };
    std::atomic<float> outputData         { 0.0f };
    bool               sineOn             { TRUE };
    bool               noiseOn            { TRUE };
    double sampleRate                     {};
    AnimatedFFT                           fft[2];

};