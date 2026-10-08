#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import "mtkViewDelegate.h"
#import "RenderBuffers.h"
#include "GameRenderer.h"
#import "../Audio/AudioRenderer.h"
#include <iostream>



@interface LpgMtkView : MTKView
- (instancetype)initWithFrame:(NSRect)frame audioRenderer:(MyRenderer *)audioRenderer;
@end

@implementation LpgMtkView
{
    MTKViewDelegate*            _viewDelegate;   // consider renaming the class, e.g. LpgRenderer
    id<MTLCommandQueue>         _commandQueue;
    id<MTLRenderPipelineState>  _solidColorPipelineState;
}

- (instancetype)initWithFrame:(NSRect)frame audioRenderer:(MyRenderer *)audioRenderer
{
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    self = [super initWithFrame:frame device:device];
    if (!self) return nil;

    self.framebufferOnly = YES;
    CGColorSpaceRef cs = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
    self.colorspace = cs;
    CGColorSpaceRelease(cs);
    self.clearColor = MTLClearColorMake(0.0, 0.0, 1.0, 1.0);

    _commandQueue = [device newCommandQueue];

    //============================================================================
    // Shader Library & Render Pipeline Setup
    //============================================================================
    NSError *error = nil;
    NSString *libPath = [NSBundle.mainBundle.resourcePath
            stringByAppendingPathComponent:@"shaders.metallib"];
    id<MTLLibrary> lib = [device newLibraryWithURL:[NSURL fileURLWithPath:libPath] error:&error];
    if (!lib) { NSLog(@"Failed to load metallib at path %@: %@", libPath, error); return nil; }

    id<MTLFunction> vfn = [lib newFunctionWithName:@"vertexMain"];
    id<MTLFunction> ffn = [lib newFunctionWithName:@"fragmentMain"];
    NSAssert(vfn && ffn, @"Missing shader functions");

    MTLRenderPipelineDescriptor *desc = [MTLRenderPipelineDescriptor new];
    desc.vertexFunction   = vfn;
    desc.fragmentFunction = ffn;
    desc.colorAttachments[0].pixelFormat = self.colorPixelFormat;

    _solidColorPipelineState = [device newRenderPipelineStateWithDescriptor:desc error:&error];
    if (!_solidColorPipelineState) { NSLog(@"Pipeline error: %@", error); return nil; }

    //============================================================================
    // Buffer Setup
    //============================================================================
    NSUInteger page = getpagesize();
    NSUInteger raw  = (NSUInteger)GLOBAL_WIDTH * GLOBAL_HEIGHT * sizeof(game_vertex);
    NSUInteger vertexBufferSize = (raw + page - 1) & ~(page - 1);

    GameRenderCommands gameRenderCommand = {};
    NSMutableArray *macVertexBuffers = [NSMutableArray arrayWithCapacity:kMaxBuffers];

    for (int i = 0; i < kMaxBuffers; i++) {
        auto *buf = (VertexBuffer *)malloc(sizeof(VertexBuffer));
        buf->vertices = (game_vertex *)mmap(nullptr, vertexBufferSize,
                                            PROT_READ | PROT_WRITE,
                                            MAP_PRIVATE | MAP_ANON, -1, 0);
        gameRenderCommand.vertexBuffer[i] = buf;

        id<MTLBuffer> mb = [device newBufferWithBytesNoCopy:buf->vertices
                                                     length:vertexBufferSize
                                                    options:MTLResourceStorageModeShared
                                                deallocator:^(void *p, NSUInteger len) { munmap(p, len); }];
        [macVertexBuffers addObject:mb];
    }

    //============================================================================
    // Delegate Setup
    //============================================================================
    _viewDelegate                    = [[MTKViewDelegate alloc] init];
    _viewDelegate.commandQueue       = _commandQueue;
    _viewDelegate.pipelineState      = _solidColorPipelineState;
    _viewDelegate.macVertexBuffers   = macVertexBuffers;
    _viewDelegate.gameRenderCommands = gameRenderCommand;
    _viewDelegate.audioRenderer      = audioRenderer;
    [_viewDelegate configureMetal];
    self.delegate = _viewDelegate;

    return self;
}

- (BOOL)acceptsFirstResponder { return YES; }

- (void) mouseDown:(NSEvent *) event
{
    std::cout << event. << std::endl;
}

- (void) mouseUp:(NSEvent *) event
{
    std::cout << "Mouse up" << std::endl;
}

- (void)scrollWheel:(NSEvent *)event
{
    std::cout << event.scrollingDeltaY << std::endl;
}

@end



//==============================================================
@interface
BtWindowDel: NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@implementation BtWindowDel
{
    NSWindow*                   _window;
    LpgMtkView*                 _metalKitView;
    MyRenderer                  _myAudioRenderer;

}

- (void)applicationDidFinishLaunching:(NSNotification *)note
{

	renderAudio(&_myAudioRenderer);

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
    _window.backgroundColor       = [NSColor blackColor];
    _window.title                 = @"Animated LPG";
    _window.delegate              = self;
    _metalKitView = [[LpgMtkView alloc] initWithFrame:NSMakeRect(0, 0, GLOBAL_WIDTH, GLOBAL_HEIGHT)
                                        audioRenderer:&_myAudioRenderer];
    _window.contentView           = _metalKitView;
    [_window makeFirstResponder:_metalKitView];
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
	stopAudio(&_myAudioRenderer);

	return YES;
}

@end

//=======================
