//
// Created by Erik Jourgensen on 9/22/26.
//
#include "RenderBuffers.h"
#import "../Geometries/SquareGeo.h"
#import "../Geometries/TriangleGeo.h"
#import "Common.h"
#import "../Audio/AudioTypes.h"

#define GLOBAL_WIDTH  512
#define GLOBAL_HEIGHT 512
static const NSUInteger kMaxBuffers = 3;
//==============================================================
@interface
MTKViewDelegate: NSObject <MTKViewDelegate>
@property (retain) id<MTLCommandQueue>        commandQueue;
@property (retain) NSMutableArray*            macVertexBuffers;
@property (retain) id<MTLRenderPipelineState> pipelineState;
@property GameRenderCommands                  gameRenderCommands;
@property Uniforms                            uniforms;
@property (nonatomic, assign) MyRenderer      *audioRenderer;
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

    uint32_t frameIndex   = _currentFrameIndex;

    uint32_t vertexCount = 0;

    game_vertex *vertices = self.gameRenderCommands.vertexBuffer[frameIndex]->vertices;



    vector_float4 blue    = { 0.0f, 0.0f, 1.0f, 1.0f };
    vector_float4 red     = { 1.0f, 0.0f, 0.0f, 1.0f };
    vector_float4 green   = { 0.0f, 1.0f, 0.0f, 1.0f };

    drawSquare(vertices,
               &vertexCount,
               64,
               blue,
               -120,
               250);

    drawSquare(vertices,
               &vertexCount,
               35,
               red,
               100,
               0);

    drawSquare(vertices,
               &vertexCount,
               100,
               green,
               300,
               0);


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

        // ==============================================
        // PASS UNIFORMS TO FRAGMENT SHADER AT INDEX 11
        // ==============================================
        _uniforms.audioData     = _audioRenderer->outputData;

        [RenderEncoder setFragmentBytes:&_uniforms
                                 length:sizeof(Uniforms)
                                atIndex:11];

        [RenderEncoder setViewport: viewPort];

        [RenderEncoder drawPrimitives: MTLPrimitiveTypeTriangle
                          vertexStart: 0
                          vertexCount: vertexCount];


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