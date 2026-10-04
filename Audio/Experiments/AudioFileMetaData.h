//
// Created by Erik Jourgensen on 9/29/26.
//

#ifndef HOMEMADELPG_AUDIOFILEMETADATA_H
#define HOMEMADELPG_AUDIOFILEMETADATA_H
#import "AudioToolbox/AudioToolbox.h"
#import "CoreFoundation/CoreFoundation.h"

//==========================================================
//AUDIO EXPERIMENT
//==========================================================
int audioFunc(const char *path) {
    @autoreleasepool {
            NSString *audioFilePath = [[NSString stringWithUTF8String:path]
            stringByExpandingTildeInPath];
            NSURL *audioURL = [NSURL fileURLWithPath:audioFilePath];
            AudioFileID audioFile;
            OSStatus theErr = noErr;
            theErr = AudioFileOpenURL((CFURLRef) CFBridgingRetain(audioURL),
            kAudioFileReadPermission,
            0,
            &audioFile);
            assert (theErr == noErr);
            UInt32 dictionarySize = 0;
            theErr = AudioFileGetPropertyInfo(audioFile,
            kAudioFilePropertyInfoDictionary,
            &dictionarySize,
            nullptr);
            assert (theErr == noErr);
            CFDictionaryRef dictionary;
            theErr = AudioFileGetProperty(audioFile,
            kAudioFilePropertyInfoDictionary,
            &dictionarySize,
            &dictionary);
            assert (theErr == noErr);
            NSLog (@"dictionary: %@", dictionary);
            CFRelease (dictionary);
            theErr = AudioFileClose (audioFile);
            assert (theErr == noErr);
    }
    return 0;
}

#endif //HOMEMADELPG_AUDIOFILEMETADATA_H
