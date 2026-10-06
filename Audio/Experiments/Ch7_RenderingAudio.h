//
// Created by Erik Jourgensen on 10/6/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "../AudioUtilities.h"
#define sineFrequency 880.0

typedef struct MySineWavePlayer
{
    AudioUnit outputUnit; //Output
    double startingFrameCount; //Phase
} MySineWavePlayer;

//============================================
//Render callback for sine wave
//============================================
OSStatus SineWaveRenderProc(void *inRefCon,
                            AudioUnitRenderActionFlags *ioActionFlags,
                            const AudioTimeStamp *inTimeStamp,
                            UInt32 inBusNumber,
                            UInt32 inNumberFrames,
                            AudioBufferList *ioData)
{
    MySineWavePlayer *player = (MySineWavePlayer*) inRefCon;
    double j                 = player->startingFrameCount;
    double cycleLength       = 44100. / sineFrequency;
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
    player->startingFrameCount = j;
    return noErr;
}

//============================================
//Function for creating an output audio unit
//============================================
void CreateAndConnectOutputUnit(MySineWavePlayer *player)
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
    input.inputProc       = SineWaveRenderProc;
    input.inputProcRefCon = &player;
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
void renderAudio()
{
    MySineWavePlayer sineWavePlayer = {0};
    //============================
    //Callback function
    //============================
    CreateAndConnectOutputUnit(&sineWavePlayer); //Callback function, good here
    printf("Hi erik");
    //============================
    //Start playback
    //============================
    checkError(AudioOutputUnitStart(sineWavePlayer.outputUnit),
               "Could not start output unit");
    sleep(5);

    cleanup:

    AudioOutputUnitStop(sineWavePlayer.outputUnit);
    AudioUnitUninitialize(sineWavePlayer.outputUnit);
    AudioComponentInstanceDispose(sineWavePlayer.outputUnit);

}

