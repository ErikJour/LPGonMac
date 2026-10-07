//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"


typedef struct MyRenderer
{
    AudioUnit outputUnit;
    double    startingFrameCount;
    float     outputData;
} MyRenderer;
