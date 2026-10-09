//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "AudioUtilities.h"
#include "DSP/TestToneGenerator.h"
#include "DSP/AnimatedGain.h"
#include "AudioTypes.h"

static bool audioOn = false;
static float gainAmount = 0.75f;


//============================================
//Render callback for sine wave
//============================================
OSStatus processBlock(void *inRefCon,
                      AudioUnitRenderActionFlags *ioActionFlags,
                      const AudioTimeStamp *inTimeStamp,
                      UInt32 inBusNumber,
                      UInt32 inNumberFrames,
                      AudioBufferList *ioData)
{
    auto *renderer = (MyRenderer*) inRefCon;

        generateTestTone(renderer, ioData, inNumberFrames);
        applyGain(renderer, ioData, inNumberFrames, gainAmount);
        //applyFilter
        //etc

        return noErr;
}

//============================================
//Function for creating an output audio unit
//============================================
void CreateAndConnectOutputUnit(MyRenderer *renderer)
{
    //============================================
    //First, define the output component
    //============================================
    AudioComponentDescription outputCd = {};
    outputCd.componentType             = kAudioUnitType_Output;
    outputCd.componentSubType          = kAudioUnitSubType_DefaultOutput;
    outputCd.componentManufacturer     = kAudioUnitManufacturer_Apple;
    //============================================
    //Second, find the audio unit output component
    //============================================
    AudioComponent comp = AudioComponentFindNext(NULL, &outputCd);
    if (comp == nullptr) {
        printf("can't get output unit");
        exit (-1);
    }

    checkError(AudioComponentInstanceNew(comp, &renderer->outputUnit),
               "Could not open component for output unit");
    //============================================
    //Third, register the render callback
    //============================================
    AURenderCallbackStruct input;
    input.inputProc       = processBlock;
    input.inputProcRefCon = renderer;
    checkError(AudioUnitSetProperty(renderer->outputUnit,
                                    kAudioUnitProperty_SetRenderCallback,
                                    kAudioUnitScope_Input,
                                    0,
                                    &input,
                                    sizeof(input)),
               "AudioUnitSetProperty failed");
    checkError (AudioUnitInitialize(renderer->outputUnit),
                "Could not initialize output unit");
}

//==========================================
//Render function to place in main
//==========================================
void renderAudio(MyRenderer* renderer)
{
    renderer->outputUnit = nullptr;
    renderer->startingFrameCount = 0.0;
    renderer->outputData.store(0.0f, std::memory_order_relaxed);
    //============================
    //Callback function
    //============================
    CreateAndConnectOutputUnit(renderer); //Callback function, good here
    //============================
    //Start playback
    //============================
    checkError(AudioOutputUnitStart(renderer->outputUnit),
               "Could not start output unit");
}

void stopAudio(MyRenderer* renderer)
{
    printf("Stopping");
    AudioOutputUnitStop(renderer->outputUnit);
    AudioUnitUninitialize(renderer->outputUnit);
    AudioComponentInstanceDispose(renderer->outputUnit);
}

void getData(MyRenderer* renderer)
{
    std::cout << &renderer->outputData << std::endl;
}
