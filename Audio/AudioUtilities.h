//
// Created by Erik Jourgensen on 10/6/26.
//

#pragma once
#include "AudioToolbox/AudioToolbox.h"

//===========================================================
//Utility functions
//===========================================================
static void checkError(OSStatus error, const char* operation)
{
    if (error == noErr) return;
    char errorString[20];
    //Check if it is a 4-char code
    *(UInt32 *) (errorString + 1) = CFSwapInt32HostToBig(error);
    if(isprint(errorString[1]) && isprint(errorString[2]) &&
       isprint(errorString[3]) && isprint(errorString[4])) {
        errorString[0] = errorString[5] = '\'';
        errorString[6] = '\0';
    } else {
        snprintf(errorString, sizeof(errorString), "%d", (int)error);
        fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
        exit(1);
    }
}
