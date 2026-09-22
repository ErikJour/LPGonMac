#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import "mtkViewDelegate.h"
//==============================================================
@interface
BtWindowDel: NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@implementation BtWindowDel
{
    NSWindow*                   _window;
	MTKView*                    _metalKitView;
	MTKViewDelegate*            _viewDelegate;
	id<MTLDevice>               _metalKitDevice;
	id<MTLCommandQueue>         _commandQueue;
	id<MTLRenderPipelineState>  ColidColorPipelineState;
}

- (void)applicationDidFinishLaunching:(NSNotification *)note
{
    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect windowRect = NSMakeRect((screenRect.size.width - GLOBAL_WIDTH) * 0.5,
                                   (screenRect.size.height - GLOBAL_HEIGHT) * 0.5,
                                   GLOBAL_WIDTH, GLOBAL_HEIGHT);
    //============================================================================
    //Window Setup
    //============================================================================
    _window = [[NSWindow alloc] initWithContentRect: windowRect
                                          styleMask: NSWindowStyleMaskTitled |
                                                     NSWindowStyleMaskClosable |
                                                     NSWindowStyleMaskMiniaturizable
                                            backing: NSBackingStoreBuffered
                                              defer: NO];

    _window.releasedWhenClosed    = NO;
    _window.minSize               = NSMakeSize(GLOBAL_WIDTH, GLOBAL_HEIGHT);
    _window.backgroundColor       = NSColor.orangeColor;
    _window.title                 = @"Animated LPG";
    _window.delegate              = self;
	_window.contentView           = _metalKitView;
    //============================================================================
    //Metal Layer Setup
    //============================================================================
	_metalKitDevice				   = MTLCreateSystemDefaultDevice();
	_commandQueue				   = [_metalKitDevice newCommandQueue];
	 _viewDelegate                 = [[MTKViewDelegate alloc] init];
	[_viewDelegate configureMetal];

	_viewDelegate.commandQueue     = _commandQueue;
	_commandQueue				   = [_metalKitDevice newCommandQueue];
	NSError *Error                 = NULL;
	NSString *libPath = [NSBundle.mainBundle.resourcePath
                             stringByAppendingPathComponent:@"shaders.metallib"];
	id<MTLLibrary> shaderLibrary = [_metalKitDevice newLibraryWithFile: libPath
														error: &Error];

	//Render pipeline
	MTLRenderPipelineDescriptor *SolidColorPipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
	ColidColorPipelineState        = [_metalKitDevice newRenderPipelineStateWithDescriptor: SolidColorPipelineDescriptor
																							error: &Error];
	id<MTLLibrary> lib = [_metalKitDevice newLibraryWithURL:[NSURL fileURLWithPath:libPath]
                                                  error:&Error];
	id<MTLFunction> vfn = [lib newFunctionWithName:@"vertexMain"];
    id<MTLFunction> ffn = [lib newFunctionWithName:@"fragmentMain"];
    NSAssert(vfn && ffn, @"Missing shader functions");
	SolidColorPipelineDescriptor.vertexFunction = vfn;
	SolidColorPipelineDescriptor.fragmentFunction = ffn;

	if (Error != NULL) {
		[NSException raise: @"Can't setup metal exception"
			format: @"Unable to setup metal pipeline state"];
	}

    _metalKitView.device           = _metalKitDevice;
    _metalKitView                  = [[MTKView alloc] initWithFrame:_window.contentLayoutRect
																	device:_metalKitDevice];
    _metalKitView.framebufferOnly  = YES;
    CGColorSpaceRef cs             = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
	_metalKitView.colorspace       = cs;
	CGColorSpaceRelease(cs);
	_metalKitView.clearColor       = MTLClearColorMake(0.0, 0.0, 1.0, 1.0); // Solid Blue
	_metalKitView.delegate         = _viewDelegate;
	_window.contentView            = _metalKitView;
    //============================================================================
    //Renderer Setup
    //============================================================================
    if ([NSUserDefaults.standardUserDefaults boolForKey:@"keepOnTop"])
    {
        _window.level = NSFloatingWindowLevel;
    }

    [_window makeKeyAndOrderFront: nil];
    [NSApp activate];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}
@end