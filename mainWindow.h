#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import "mtkViewDelegate.h"
#import "RenderBuffers.h"
#include "GameRenderer.h"
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
	id<MTLRenderPipelineState>  solidColorPipelineState;
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
	_window.backgroundColor = [NSColor blackColor];
    _window.title                 = @"Animated LPG";
    _window.delegate              = self;
	_window.contentView           = _metalKitView;
	//============================================================================
	// Shader Library & Render Pipeline Setup
	//============================================================================
	NSError *Error                 = nullptr;

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
	SolidColorPipelineDescriptor.colorAttachments[0].pixelFormat = _metalKitView.colorPixelFormat;
	solidColorPipelineState                       = [_metalKitDevice newRenderPipelineStateWithDescriptor:
					SolidColorPipelineDescriptor
																									error: &Error];
	if (!solidColorPipelineState) {
		[NSException raise:@"Can't setup metal exception"
					format:@"Unable to setup metal pipeline state: %@", Error];
	}

	if (Error != nullptr) {
		[NSException raise: @"Can't setup metal exception"
					format: @"Unable to setup metal pipeline state"];
	}
	//============================================================================
    //Buffer Setup
    //============================================================================
    VertexBuffer gameVertexBuffer        = {};
	uint32_t pageSize         			= GLOBAL_WIDTH * GLOBAL_HEIGHT;
	uint32_t vertexBufferSize 			= pageSize * sizeof(game_vertex); // Size by struct count, not magic 1000 multiplier
	GameRenderCommands gameRenderCommand = {};
    NSMutableArray *macVertexBuffers     = [[NSMutableArray alloc] init];

	for (int i = 0; i < kMaxBuffers; i++) {
		// 1. Allocate memory for this buffer's struct
		auto *buf = (VertexBuffer *)malloc(sizeof(VertexBuffer));

		// 2. Map virtual memory page for the vertex array
		buf->vertices = (game_vertex *)mmap(nullptr,
											vertexBufferSize,
											PROT_READ | PROT_WRITE,
											MAP_PRIVATE | MAP_ANON,
											-1,
											0);

		// 3. Save struct pointer back into gameRenderCommand array
		gameRenderCommand.vertexBuffer[i] = buf;

		// 4. Wrap mapped memory into Metal MTLBuffer without copying
		id<MTLBuffer> MetalVertexBuffer = [_metalKitDevice newBufferWithBytesNoCopy: buf->vertices
																			 length: vertexBufferSize
																			options: MTLResourceStorageModeShared
																		deallocator: ^(void *pointer, NSUInteger length) {
																			munmap(pointer, length);
																		}];

		[macVertexBuffers addObject: MetalVertexBuffer];
	}
    //============================================================================
    //Delegate Setup
    //============================================================================
    _viewDelegate                    = [[MTKViewDelegate alloc] init];
    _viewDelegate.macVertexBuffers   = macVertexBuffers;
    _viewDelegate.gameRenderCommands = gameRenderCommand;
    _viewDelegate.pipelineState      = solidColorPipelineState;
	[_viewDelegate configureMetal];
	_viewDelegate.commandQueue       = _commandQueue;
	_commandQueue				     = [_metalKitDevice newCommandQueue];
	_metalKitView.delegate           = _viewDelegate;

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