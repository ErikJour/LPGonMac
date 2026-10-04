#import <AppKit/AppKit.h>
#import "mainWindow.h"
#import "Audio/Ch2_SquareWave.h"
#import "Audio/Ch2_SawtoothWave.h"
//===========================================================
//Main Loop
//===========================================================
static BtWindowDel *btWindowDelegate = nil;

int main(int argc, const char *argv[])
{
    NSApplication *app = [NSApplication sharedApplication];
    btWindowDelegate   = [[BtWindowDel alloc] init];
    app.delegate       = btWindowDelegate;

    return NSApplicationMain(argc, argv);
}