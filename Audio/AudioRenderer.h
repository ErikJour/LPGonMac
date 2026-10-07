//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "AudioUtilities.h"
#include "DSP/TestToneGenerator.h"
#include "AudioTypes.h"

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
    MyRenderer *player = (MyRenderer*) inRefCon; //This might be an issue

    generateTestTone(player, ioData, inNumberFrames);
    //applyGain
    //applyFilter
    //etc

    return noErr;
}

//============================================
//Function for creating an output audio unit
//============================================
void CreateAndConnectOutputUnit(MyRenderer *player)
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

    checkError(AudioComponentInstanceNew(comp, &player->outputUnit),
               "Could not open component for output unit");
    //============================================
    //Third, register the render callback
    //============================================
    AURenderCallbackStruct input;
    input.inputProc       = processBlock;
    input.inputProcRefCon = player;
    checkError(AudioUnitSetProperty(player->outputUnit,
                                    kAudioUnitProperty_SetRenderCallback,
                                    kAudioUnitScope_Input,
                                    0,
                                    &input,
                                    sizeof(input)),
               "AudioUnitSetProperty failed");
    checkError (AudioUnitInitialize(player->outputUnit),
                "Could not initialize output unit");
}

//==========================================
//Render function to place in main
//==========================================
void renderAudio(MyRenderer* sineWavePlayer)
{
    *sineWavePlayer = {0};
    //============================
    //Callback function
    //============================
    CreateAndConnectOutputUnit(sineWavePlayer); //Callback function, good here
    printf("Hi erik");
    //============================
    //Start playback
    //============================
    checkError(AudioOutputUnitStart(sineWavePlayer->outputUnit),
               "Could not start output unit");
}

void stopAudio(MyRenderer* player)
{
    printf("Stopping");
    AudioOutputUnitStop(player->outputUnit);
    AudioUnitUninitialize(player->outputUnit);
    AudioComponentInstanceDispose(player->outputUnit);
}
