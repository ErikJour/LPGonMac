#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#define GLOBAL_WIDTH  512
#define GLOBAL_HEIGHT 288
static const NSUInteger kMaxBuffers = 3;
//==============================================================
@interface
MTKViewDelegate: NSObject <MTKViewDelegate>
@property id<MTLCommandQueue> commandQueue;
@end

@implementation MTKViewDelegate
{
    dispatch_semaphore_t     _frameBoundarySemaphore;
	uint32_t      			 _currentFrameIndex;
}

-(void)configureMetal
{
_frameBoundarySemaphore = dispatch_semaphore_create(kMaxBuffers);
_currentFrameIndex      = 0;
}

-(void)mtkView:(MTKView *) view drawableSizeWillChange:(CGSize) size
{

}

- (void)drawInMTKView:(MTKView *) view
{
	dispatch_semaphore_wait(_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);

	MTLViewport viewPort = { 0, 0, GLOBAL_WIDTH, GLOBAL_HEIGHT };

	@autoreleasepool{

	id<MTLCommandBuffer> CommandBuffer = [self.commandQueue commandBuffer];

	MTLRenderPassDescriptor *RenderPassDescriptor       = [view currentRenderPassDescriptor];
	RenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
	MTLClearColor MetalClearColor                       = MTLClearColorMake(0.0f, 255.0f, 0.0f, 1.0f);
	RenderPassDescriptor.colorAttachments[0].clearColor = MetalClearColor;

	id<MTLRenderCommandEncoder> RenderEncoder = [CommandBuffer renderCommandEncoderWithDescriptor:RenderPassDescriptor];
	RenderEncoder.label = @"RenderEncoder";
	[RenderEncoder setViewport: viewPort];
	[RenderEncoder endEncoding];

	id<CAMetalDrawable> NextDrawable = [view currentDrawable];
	[CommandBuffer presentDrawable:NextDrawable];

	__block dispatch_semaphore_t semaphore = _frameBoundarySemaphore;

	[CommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
		dispatch_semaphore_signal(semaphore);
	}];

	[CommandBuffer commit];
	}
}

@end
//==============================================================
@interface
BtWindowDel: NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

//Member variables
@implementation BtWindowDel
{
    // The on-screen window: title bar, frame, screen position
    NSWindow*                   _window;
	MTKView*                    _metalKitView;
	MTKViewDelegate*            _viewDelegate;
	id<MTLDevice>               _metalKitDevice;
	id<MTLCommandQueue>         _commandQueue;
	id<MTLRenderPipelineState>  ColidColorPipelineState;
}
//===========================================================
//This is our initialization, called by OS via main in NSApplication
//===========================================================
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