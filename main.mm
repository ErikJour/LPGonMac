#pragma once
#import <AppKit/AppKit.h>
#import "mainWindow.h"
#import "Audio/Experiments/Ch2_SquareWave.h"
#import "Audio/Experiments/Ch2_SawtoothWave.h"
#include "AudioToolbox/AudioToolbox.h"
#define kNumberRecordBuffers 3

//===========================================================
//Audio Recorder
//===========================================================
//Data Structures
typedef struct MyRecorder {
    AudioFileID recordFile;
    SInt64      recordPacket;
    Boolean     running;
} MyRecorder;

//Utility functions
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
        sprintf(errorString, "%d", (int)error);
        fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
        exit(1);
    }
}
OSStatus MyGetDefaultInputDeviceSampleRate(Float64 *outSampleRate) {
    OSStatus error;
    AudioDeviceID deviceID = {};
    AudioObjectPropertyAddress propertyAddress;
    UInt32 propertySize;
    propertyAddress.mSelector = kAudioHardwarePropertyDefaultInputDevice;
    propertyAddress.mScope    = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement  = 0;
    propertySize = sizeof(AudioDeviceID);
    error = AudioHardwareServiceGetPropertyData(kAudioObjectSystemObject,
                                                &propertyAddress,
                                                0,
                                                nullptr,
                                                &propertySize,
                                                &deviceID);

};
static int MyComputeRecordBufferSize(const AudioStreamBasicDescription *format, AudioQueueRef queue, float seconds) {};
static void MyCopyEncoderCookieToFile(AudioQueueRef queue, AudioFileID theFile) {};
//Callback function

//===========================================================
//Main Loop
//===========================================================
static BtWindowDel *btWindowDelegate = nil;

int main(int argc, const char *argv[])
{
    NSApplication *app = [NSApplication sharedApplication];
    btWindowDelegate   = [[BtWindowDel alloc] init];
    app.delegate       = btWindowDelegate;
    //======================================================
    //set up format
    MyRecorder recorder = {};
    AudioStreamBasicDescription recordFormat;
    memset(&recordFormat, 0, sizeof(recordFormat));
    recordFormat.mFormatID         = kAudioFormatMPEG4AAC;
    recordFormat.mChannelsPerFrame = 2;
    MyGetDefaultInputDeviceSampleRate(&recordFormat.mSampleRate);
    UInt32  propSize = sizeof(recordFormat);
    checkError(AudioFormatGetProperty(kAudioFormatProperty_FormatInfo,
                                      0,
                                      NULL,
                                      &propSize,
                                      &recordFormat),
                                      "AudioFormatGetProperty failed");
    //==================================================
    //set up queue
    //==================================================
    AudioQueueRef queue = nullptr;
    checkError(AudioQueueNewInput(&recordFormat,
                                  /*MyAQInputCallback*/,
                                  &recorder,
                                  NULL,
                                  NULL,
                                  0,
                                  &queue),
                                "AudioQueueNewINput failed");

    UInt32 size = sizeof(recordFormat);
    checkError(AudioQueueGetProperty(queue,
                                     kAudioConverterCurrentOutputStreamDescription,
                                     &recordFormat,
                                     &size),
                                    "Couldn't get queue's format");
    CFURLRef myFileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                       CFSTR("output.caf"),
                                                       kCFURLPOSIXPathStyle,
                                                       false);
    checkError(AudioFileCreateWithURL(myFileURL,
               kAudioFileCAFType,
               &recordFormat,
               kAudioFileFlags_EraseFile,
               &recorder.recordFile),
               "AudioFileCreateWithURL failed");
    CFRelease(myFileURL);
    MyCopyEncoderCookieToFile(queue, recorder.recordFile);
    //Other setup
    int bufferByteSize = MyComputeRecordBufferSize(&recordFormat, queue, 0.5);
    int bufferIndex;
    for (bufferIndex = 0; bufferIndex < kNumberRecordBuffers; ++bufferIndex) {
        AudioQueueBufferRef buffer;
        checkError(AudioQueueAllocateBuffer(queue, bufferByteSize, &buffer), "AudioQuueAllocateBuffer failed");
        checkError(AudioQueueEnqueueBuffer(queue,
                                                buffer,
                                                0,
                                                nullptr),
                                                "AudioQueueEnqueueBuffer failed");
    }
    //==================================================
    //Start queue
    //==================================================
    recorder.running = true;
    checkError(AudioQueueStart(queue, nullptr), "AudioQueueStart failed");
    printf("Recording, press <return> to stop:\n");
    getchar();
    //==================================================
    //Stop queue
    //======================================================
    printf("Recording done *\n");
    recorder.running = false;
    checkError(AudioQueueStop(queue, TRUE), "AudioQueueStop failed");
    MyCopyEncoderCookieToFile(queue, recorder.recordFile);
    AudioQueueDispose(queue, TRUE);
    AudioFileClose(recorder.recordFile);
    //======================================================
    return NSApplicationMain(argc, argv);
}