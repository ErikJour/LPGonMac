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

    uint32_t frameIndex = _currentFrameIndex;

    game_vertex *vertices = self.gameRenderCommands.vertexBuffer[frameIndex]->vertices;
    //==========
    //Triangle
    //==========
    vertices[0] = { { 0.0f, 1.0f, 0.0f, 1.0f }, {0.0f, 0.0f, 1.0f, 1.0f }  };
    vertices[1] = { { -1.0f, -1.0f, 0.0f, 1.0f }, {0.0f, 1.0f, 0.0f, 1.0f }  };
    vertices[2] = { { 1.0f, -1.0f, 0.0f, 1.0f }, {1.0f, 0.0f, 0.0f, 1.0f }  };

    @autoreleasepool{

        id<MTLCommandBuffer> CommandBuffer = [self.commandQueue commandBuffer];

        MTLRenderPassDescriptor *RenderPassDescriptor       = [view currentRenderPassDescriptor];
        RenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        RenderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.1f, 0.1f, 0.1f, 1.0f);

        id<MTLRenderCommandEncoder> RenderEncoder = [CommandBuffer renderCommandEncoderWithDescriptor:RenderPassDescriptor];
        RenderEncoder.label = @"RenderEncoder";

        // Viewport using actual drawable size
        CGSize drawableSize = view.drawableSize;
        MTLViewport viewPort = { 0.0, 0.0, drawableSize.width, drawableSize.height, 0.0, 1.0 };
        [RenderEncoder setViewport: viewPort];

        [RenderEncoder setRenderPipelineState: [self pipelineState]];
        [RenderEncoder setVertexBuffer: [[self macVertexBuffers] objectAtIndex: _currentFrameIndex]
                                offset: 0
                                 atIndex: 0];

        [RenderEncoder setViewport: viewPort];

        [RenderEncoder drawPrimitives: MTLPrimitiveTypeTriangle
                          vertexStart: 0
                          vertexCount: 3];


        [RenderEncoder endEncoding];

        id<CAMetalDrawable> NextDrawable = [view currentDrawable];
        [CommandBuffer presentDrawable:NextDrawable];

        uint32_t nextIndex = _currentFrameIndex + 1;
        if (nextIndex > 2) {
            nextIndex = 0;
        }

        _currentFrameIndex = nextIndex;


        __block dispatch_semaphore_t semaphore = _frameBoundarySemaphore;

        [CommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
            dispatch_semaphore_signal(semaphore);
        }];

        [CommandBuffer commit];
    }
}

@end