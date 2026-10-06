#pragma once
#import <AppKit/AppKit.h>
#import "mainWindow.h"
#import "Audio/Experiments/Ch7_RenderingAudio.h"

//===========================================================
//Main Loop
//===========================================================
static BtWindowDel *btWindowDelegate = nil;

int main(int argc, const char *argv[])
{
    NSApplication *app = [NSApplication sharedApplication];
    btWindowDelegate   = [[BtWindowDel alloc] init];
    app.delegate       = btWindowDelegate;
    renderAudio();


    return NSApplicationMain(argc, argv);
}