//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <atomic>

struct MyRenderer
{
    AudioUnit          outputUnit{nullptr};
    double             startingFrameCount{0.0};
    std::atomic<float> outputData{0.0f};
};