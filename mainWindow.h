#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import "mtkViewDelegate.h"
#import "RenderBuffers.h"
#include "GameRenderer.h
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
	id<MTLRenderPipelineState>  SolidColorPipelineState;
}

- (void)applicationDidFinishLaunching:(NSNotification *)note
{
    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect windowRect = NSMakeRect((screenRect.size.width - GLOBAL_WIDTH) * 0.5,
                                   (screenRect.size.height - GLOBAL_HEIGHT) * 0.5,
                                   GLOBAL_WIDTH, GLOBAL_HEIGHT);
	//=============================================================================
	//Metal Setup
	//=============================================================================
	_metalKitDevice				   = MTLCreateSystemDefaultDevice();
	_commandQueue				   = [_metalKitDevice newCommandQueue];
	_metalKitView                  = [[MTKView alloc] initWithFrame:_window.contentLayoutRect
																	device:_metalKitDevice];
	_metalKitView.framebufferOnly  = YES;
    CGColorSpaceRef cs             = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
	_metalKitView.colorspace       = cs;
	CGColorSpaceRelease(cs);
	_metalKitView.clearColor       = MTLClearColorMake(0.0, 0.0, 1.0, 1.0); // Solid Blue
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
    //Buffer Setup
    //============================================================================
	MacVertexBuffers macVertexBuffers    = {};
	u32 pageSize                         = GLOBAL_WIDTH * GLOBAL_HEIGHT;
	u32 vertexBufferSize                 = pageSize * 1000;
	GameRenderCommands gameRenderCommand = {};

	for (u32 FrameIndex = 0;
			FrameIndex< 3;
			FrameIndex++) {
			gameRenderCommand.vertexBuffer[FrameIndex] = (game_vertex_buffer *)mmap(0,
																					vertexBufferSize,
																					PROT_READ |
																					PROT_WRITE |
																					MAP_PRIVATE |
																					MAP_ANON,
																					-1,
																					0);
			id<MTLBuffer> MetalVertexBuffer = [_metalKitView.device newBufferWithBytesNoCopy:      vertices,
																					length:      vertexBufferSize,
																					options:     MTLResourceStorageModeShared,
																					deallocator: nil];
			macVertexBuffers.MetalVertexBuffers[FrameIndex] = FrameIndex;

	}
    //============================================================================
    //Delegate Setup
    //============================================================================
	 _viewDelegate                 = [[MTKViewDelegate alloc] init];
	[_viewDelegate configureMetal];

	_viewDelegate.commandQueue     = _commandQueue;
	_commandQueue				   = [_metalKitDevice newCommandQueue];
	_metalKitView.delegate         = _viewDelegate;
	//============================================================================
    // Shader Library & Render Pipeline Setup
    //============================================================================
	NSError *Error                 = NULL;

	NSString *libPath = [NSBundle.mainBundle.resourcePath
                             stringByAppendingPathComponent:@"shaders.metallib"];

	id<MTLLibrary> lib = [_metalKitDevice newLibraryWithURL:[NSURL fileURLWithPath:libPath]
                                                  error:&Error];
	if (!lib) {
        NSLog(@"Failed to load metallib at path %@: %@", libPath, Error);
        return;
    }
	id<MTLFunction> vfn = [lib newFunctionWithName:@"vertexMain"];
    id<MTLFunction> ffn = [lib newFunctionWithName:@"fragmentMain"];
	NSAssert(vfn && ffn, @"Missing shader functions: check vertexMain and fragmentFunction");
	MTLRenderPipelineDescriptor *SolidColorPipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
	SolidColorPipelineDescriptor.vertexFunction   = vfn;
	SolidColorPipelineDescriptor.fragmentFunction = ffn;
	SolidColorPipelineState        = [_metalKitDevice newRenderPipelineStateWithDescriptor: SolidColorPipelineDescriptor
																							error: &Error];
	if (!SolidColorPipelineState) {
        [NSException raise:@"Can't setup metal exception"
                    format:@"Unable to setup metal pipeline state: %@", Error];
    }

	if (Error != NULL) {
		[NSException raise: @"Can't setup metal exception"
			format: @"Unable to setup metal pipeline state"];
	}
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