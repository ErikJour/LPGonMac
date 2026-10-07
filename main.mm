#import <AppKit/AppKit.h>
#import "UI/mainWindow.h"

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