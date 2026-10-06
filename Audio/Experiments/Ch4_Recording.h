//
// Created by Erik Jourgensen on 10/5/26.
//

#pragma once
#import "AudioToolbox/AudioToolbox.h"
#include "../AudioUtilities.h"
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
    if (error) return error;
    propertyAddress.mSelector = kAudioDevicePropertyNominalSampleRate;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = 0;
    propertySize = sizeof(Float64);
    error = AudioHardwareServiceGetPropertyData(deviceID,
                                                &propertyAddress,
                                                0,
                                                nullptr,
                                                &propertySize,
                                                outSampleRate);
    return error;
}

static int MyComputeRecordBufferSize(const AudioStreamBasicDescription *format, AudioQueueRef queue, float seconds) {
    int packets, frames, bytes;
    frames = (int)ceil(seconds * format->mSampleRate);

    if (format->mBytesPerFrame > 0)
        bytes = frames * format->mBytesPerFrame;
    else {
        UInt32 maxPacketSize;
        if (format->mBytesPerPacket > 0 )
            maxPacketSize = format->mBytesPerPacket;
        else {
            UInt32 propertySize = sizeof(maxPacketSize);
            checkError(AudioQueueGetProperty(queue,
                                             kAudioConverterPropertyMaximumOutputPacketSize,
                                             &maxPacketSize,
                                             &propertySize),
                       "Could not get queue's maximum output packet size");
        }
        if (format->mFramesPerPacket > 0)
            packets = frames / format->mFramesPerPacket;
        else
            packets = frames;

        if (packets == 0)
            packets = 1;
        bytes = packets * maxPacketSize;
    }
    return bytes;
}
static void MyCopyEncoderCookieToFile(AudioQueueRef queue, AudioFileID theFile) {
    OSStatus error;
    UInt32 propertySize;
    error = AudioQueueGetPropertySize(queue,
                                      kAudioConverterCompressionMagicCookie,
                                      &propertySize);
    if (error == noErr && propertySize > 0) {
        Byte *magicCookie = (Byte *) malloc(propertySize);
        checkError(AudioQueueGetProperty(queue, kAudioQueueProperty_MagicCookie,
                                         magicCookie,
                                         &propertySize),
                   "Could not get audio file's magic cookie");
        checkError(AudioFileSetProperty(theFile,
                                        kAudioFilePropertyMagicCookieData,
                                        propertySize,
                                        magicCookie),
                   "Could not set audio file's magic cookie");
        free(magicCookie);
    }
};
//Callback function
static void MYAQInputCallback(void *inUserData,
                              AudioQueueRef inQueue,
                              AudioQueueBufferRef inBuffer,
                              const AudioTimeStamp *inStartTime,
                              UInt32 inNumPackets,
                              const AudioStreamPacketDescription *inPacketDesc)
{
    auto *recorder = (MyRecorder *)inUserData; //Cast userData to MyRecorder struct
    if (inNumPackets > 0) {
        checkError(AudioFileWritePackets(recorder->recordFile,
                                         FALSE,
                                         inBuffer->mAudioDataByteSize,
                                         inPacketDesc,
                                         recorder->recordPacket,
                                         &inNumPackets,
                                         inBuffer->mAudioData),
                   "AudioFileWritePackets failed");
        recorder->recordPacket += inNumPackets;
    }
    if (recorder->running)
        checkError(AudioQueueEnqueueBuffer(inQueue,
                                           inBuffer,
                                           0,
                                           nullptr),
                   "AudioQueueEnqueueBuffer failed");

}

inline void record()
{
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
                                  MYAQInputCallback,
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
}

