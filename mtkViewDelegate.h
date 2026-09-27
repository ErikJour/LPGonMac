//
// Created by Erik Jourgensen on 9/22/26.
//
#include "RenderBuffers.h"
#define GLOBAL_WIDTH  512
#define GLOBAL_HEIGHT 288
static const NSUInteger kMaxBuffers = 3;
//==============================================================
@interface
MTKViewDelegate: NSObject <MTKViewDelegate>
@property (retain) id<MTLCommandQueue>        commandQueue;
@property (retain) NSMutableArray*            macVertexBuffers;
@property (retain) id<MTLRenderPipelineState> pipelineState;
@property GameRenderCommands                  gameRenderCommands;
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
//        [RenderEncoder setRenderPipelineState: [self pipelineState]];
//        [RenderEncoder setVertexBuffer: [self macVertexBuffers] objectAtIndex: [self currentFrameIndex]
//                                offset: 0
//                                 atIndex: 0];


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