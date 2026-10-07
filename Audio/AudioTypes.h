//
// Created by Erik Jourgensen on 10/7/26.
//

// AudioTypes.h
#pragma once
#include <AudioToolbox/AudioToolbox.h>

#if defined(__cplusplus)
#include <atomic>
typedef std::atomic<float> AtomicFloat;
#else
#include <stdatomic.h>
  typedef _Atomic float AtomicFloat;
#endif

typedef struct MyRenderer
{
    AudioUnit   outputUnit;
    double      startingFrameCount;
    AtomicFloat outputData;
} MyRenderer;