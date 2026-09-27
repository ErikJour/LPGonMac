#import <AppKit/AppKit.h>
#import "mainWindow.h"
#import "AudioToolbox/AudioToolbox.h"
#import "CoreFoundation/CoreFoundation.h"
//==========================================================
//AUDIO EXPERIMENT
//==========================================================
int audioFunc(const char *path) {
    @autoreleasepool {
//        if (argc < 2) {
//            printf("Usage: CAMetadata /full/path/to/audiofile\n");
//            return -1;
//        }
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




//===========================================================
//Main Loop
//===========================================================
static BtWindowDel *btWindowDelegate = nil;

int main(int argc, const char *argv[])
{
    NSApplication *app = [NSApplication sharedApplication];
    btWindowDelegate   = [[BtWindowDel alloc] init];
    app.delegate       = btWindowDelegate;
    char buf[]         = "/Users/erikjourgensen/Desktop/132586__rob10__kick-drum-f.wav";
//    char *bufPointer   = buf;
    audioFunc( buf);

    return NSApplicationMain(argc, argv);
}