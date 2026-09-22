//
// Created by Erik Jourgensen on 9/22/26.
//
#define GLOBAL_WIDTH  512
#define GLOBAL_HEIGHT 288
static const NSUInteger kMaxBuffers = 3;
//==============================================================
@interface
MTKViewDelegate: NSObject <MTKViewDelegate>
@property id<MTLCommandQueue> commandQueue;
@end

//==============================================================
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

        //Prepping the render happens here ->

        //===================================

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