/**

MIT License

Copyright (c) 2022 Mitchell Davis <mdavisprog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "Interface.h"
#include "OctaneUI/OctaneUI.h"
#include "SDL.h"

#include <unordered_map>
#include <vector>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

namespace SDL
{

struct Container
{
public:
	Container()
		: Window(nullptr)
		, Renderer(nullptr)
	{}

	~Container()
	{}

	SDL_Window* Window;
	SDL_Renderer* Renderer;
};

struct TextureID
{
public:
	TextureID()
		: ID(0)
		, Texture()
	{}

	TextureID(id<MTLTexture> InTexture)
		: ID(++GlobalID)
		, Texture(InTexture)
	{}

	~TextureID()
	{}

	uint32_t ID;
	id<MTLTexture> Texture;

private:
	static uint32_t GlobalID;
};

uint32_t TextureID::GlobalID = 0;

std::unordered_map<OctaneUI::Window*, Container> Windows;
std::vector<TextureID> Textures;
id<MTLDevice> g_Device;
id<MTLCommandQueue> g_Queue;
id<MTLDepthStencilState> g_DepthStencil;
id<MTLRenderPipelineState> g_RenderPipelineTextured;
id<MTLRenderPipelineState> g_RenderPipeline;
id<MTLBuffer> g_VertexBuffer;
id<MTLBuffer> g_IndexBuffer;
MTLRenderPassDescriptor* g_RenderPass;

// The rendering code path used in this implementation is copied from the DearImGui metal implementation
// with some slight adjustments made to fit this project's needs.
// TODO: If multiple window support is implemented, give back to DearImGui project with changes.
const char* Shader = R"(#include <metal_stdlib>
using namespace metal;

struct Uniforms
{
	float4x4 ProjectionMatrix;
};

struct VertexIn
{
	float2 Position [[attribute(0)]];
	float2 TexCoords [[attribute(1)]];
	uchar4 Color [[attribute(2)]];
};

struct VertexOut
{
	float4 Position [[position]];
	float2 TexCoords;
	float4 Color;
};

vertex VertexOut Vertex_Main(VertexIn In [[stage_in]], constant Uniforms& InUniforms [[buffer(1)]])
{
	VertexOut Result;
	Result.Position = InUniforms.ProjectionMatrix * float4(In.Position, 0, 1);
	Result.TexCoords = In.TexCoords;
	Result.Color = float4(In.Color) / float4(255.0);
	return Result;
}

fragment half4 Fragment_Textured(VertexOut In [[stage_in]], texture2d<half, access::sample> Texture [[texture(0)]])
{
	constexpr sampler LinearSampler(coord::normalized, min_filter::linear, mag_filter::linear, mip_filter::linear);
	half4 TexColor = Texture.sample(LinearSampler, In.TexCoords);
	return half4(In.Color) * TexColor;
}

fragment half4 Fragment(VertexOut In [[stage_in]])
{
	return half4(In.Color);
}
)";

id<MTLTexture> GetTexture(uint32_t ID)
{
	for (const TextureID& Texture : Textures)
	{
		if (Texture.ID == ID)
		{
			return Texture.Texture;
		}
	}

	return nullptr;
}

OctaneUI::Mouse::Button GetMouseButton(uint8_t Button)
{
	switch (Button)
	{
	case SDL_BUTTON_RIGHT: return OctaneUI::Mouse::Button::Right;
	case SDL_BUTTON_MIDDLE: return OctaneUI::Mouse::Button::Middle;
	case SDL_BUTTON_LEFT:
	default: break;
	}

	return OctaneUI::Mouse::Button::Left;
}

void DestroyWindow(const Container& InContainer)
{
	SDL_DestroyWindow(InContainer.Window);
	SDL_DestroyRenderer(InContainer.Renderer);
}

// TODO: Maybe check for any failures and return a result?
void InitializeDevice(id<MTLDevice> Device)
{
	g_Queue = [g_Device newCommandQueue];
	g_RenderPass = [MTLRenderPassDescriptor new];

	MTLDepthStencilDescriptor* DepthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
	DepthStencilDesc.depthWriteEnabled = NO;
	DepthStencilDesc.depthCompareFunction = MTLCompareFunctionAlways;
	g_DepthStencil = [g_Device newDepthStencilStateWithDescriptor:DepthStencilDesc];

	NSError* Error = nullptr;
	NSString* ShaderSource = [NSString stringWithUTF8String:Shader];
	id<MTLLibrary> Library = [g_Device newLibraryWithSource:ShaderSource options:nil error:&Error];
	if (Library == nullptr)
	{
		printf("Failed to initialize metal library: %s\n", [Error.localizedDescription UTF8String]);
	}

	id<MTLFunction> VertexFn = [Library newFunctionWithName:@"Vertex_Main"];
	id<MTLFunction> FragmentTexturedFn = [Library newFunctionWithName:@"Fragment_Textured"];
	id<MTLFunction> FragmentFn = [Library newFunctionWithName:@"Fragment"];

	if (VertexFn == nullptr || FragmentFn == nullptr || FragmentTexturedFn == nullptr)
	{
		printf("Failed to find metal shader functions.\n");
	}

	MTLVertexDescriptor* VertexDesc = [MTLVertexDescriptor vertexDescriptor];
	VertexDesc.attributes[0].offset = offsetof(OctaneUI::Vertex, Position);
	VertexDesc.attributes[0].format = MTLVertexFormatFloat2;
	VertexDesc.attributes[0].bufferIndex = 0;
	VertexDesc.attributes[1].offset = offsetof(OctaneUI::Vertex, TexCoords);
	VertexDesc.attributes[1].format = MTLVertexFormatFloat2;
	VertexDesc.attributes[1].bufferIndex = 0;
	VertexDesc.attributes[2].offset = offsetof(OctaneUI::Vertex, Col);
	VertexDesc.attributes[2].format = MTLVertexFormatUChar4;
	VertexDesc.attributes[2].bufferIndex = 0;
	VertexDesc.layouts[0].stepRate = 1;
	VertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
	VertexDesc.layouts[0].stride = sizeof(OctaneUI::Vertex);

	MTLRenderPipelineDescriptor* PipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
	PipelineDesc.vertexFunction = VertexFn;
	PipelineDesc.fragmentFunction = FragmentTexturedFn;
	PipelineDesc.vertexDescriptor = VertexDesc;
	PipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
	PipelineDesc.colorAttachments[0].blendingEnabled = YES;
	PipelineDesc.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
	PipelineDesc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
	PipelineDesc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
	PipelineDesc.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
	PipelineDesc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
	PipelineDesc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

	g_RenderPipelineTextured = [g_Device newRenderPipelineStateWithDescriptor:PipelineDesc error:&Error];
	if (Error != nullptr)
	{
		printf("Failed to initialize g_RenderPipelineTextured MTLRenderPipelineState: %s\n", [Error.localizedDescription UTF8String]);
	}

	PipelineDesc.fragmentFunction = FragmentFn;
	g_RenderPipeline = [g_Device newRenderPipelineStateWithDescriptor:PipelineDesc error:&Error];
	if (Error != nullptr)
	{
		printf("Failed to initialize g_RenderPipeline MTLRenderPipelineState: %s\n", [Error.localizedDescription UTF8String]);
	}

	g_VertexBuffer = [g_Device newBufferWithLength:1024 * 1024 options:MTLResourceStorageModeShared];
	g_IndexBuffer = [g_Device newBufferWithLength:1024 * 1024 options:MTLResourceStorageModeShared];
}

void OnCreateWindow(OctaneUI::Window* Window)
{
	SDL_Window* Instance = SDL_CreateWindow(
		Window->GetTitle(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		(int)Window->GetSize().X,
		(int)Window->GetSize().Y,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
	);

	if (Instance == nullptr)
	{
		return;
	}

	SDL_Renderer* Renderer = SDL_CreateRenderer(
		Instance,
		-1,
		SDL_RENDERER_ACCELERATED
	);

	if (Renderer == nullptr)
	{
		SDL_DestroyWindow(Instance);
		return;
	}

	CAMetalLayer* Layer = (__bridge CAMetalLayer*)SDL_RenderGetMetalLayer(Renderer);
	Layer.pixelFormat = MTLPixelFormatBGRA8Unorm;

	Container& Item = Windows[Window];
	Item.Window = Instance;
	Item.Renderer = Renderer;

	if (g_Device == nullptr)
	{
		g_Device = Layer.device;
		InitializeDevice(g_Device);
	}
}

void OnDestroyWindow(OctaneUI::Window* Window)
{
	if (Windows.find(Window) == Windows.end())
	{
		return;
	}

	DestroyWindow(Windows[Window]);
	Windows.erase(Window);
}

OctaneUI::Event OnEvent(OctaneUI::Window* Window)
{
	if (Windows.find(Window) == Windows.end())
	{
		return OctaneUI::Event(OctaneUI::Event::Type::WindowClosed);
	}

	const Container& Item = Windows[Window];
	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
		case SDL_QUIT: return OctaneUI::Event(OctaneUI::Event::Type::WindowClosed);
		case SDL_MOUSEMOTION: return OctaneUI::Event(
			OctaneUI::Event::MouseMove(Event.motion.x, Event.motion.y)
		);
		case SDL_MOUSEBUTTONDOWN: return OctaneUI::Event(
			OctaneUI::Event::Type::MousePressed,
			OctaneUI::Event::MouseButton(GetMouseButton(Event.button.button), (float)Event.button.x, (float)Event.button.y)
		);
		case SDL_MOUSEBUTTONUP: return OctaneUI::Event(
			OctaneUI::Event::Type::MouseReleased,
			OctaneUI::Event::MouseButton(GetMouseButton(Event.button.button), (float)Event.button.x, (float)Event.button.y)
		);
		default: break;
		}
	}

	return OctaneUI::Event(OctaneUI::Event::Type::None);
}

void OnPaint(OctaneUI::Window* Window, const std::vector<OctaneUI::VertexBuffer>& Buffers)
{
	if (Windows.find(Window) == Windows.end())
	{
		return;
	}

	const Container& Item = Windows[Window];
	CAMetalLayer* Layer = (__bridge CAMetalLayer*)SDL_RenderGetMetalLayer(Item.Renderer);

	@autoreleasepool
	{
		id<CAMetalDrawable> Surface = [Layer nextDrawable];

		MTLRenderPassDescriptor* Descriptor = g_RenderPass;
		Descriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
		Descriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
		Descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
		Descriptor.colorAttachments[0].texture = Surface.texture;

		id<MTLCommandBuffer> Buffer = [g_Queue commandBuffer];
		id<MTLRenderCommandEncoder> Encoder = [Buffer renderCommandEncoderWithDescriptor:Descriptor];

		int Width, Height;
		SDL_GetRendererOutputSize(Item.Renderer, &Width, &Height);
		Layer.drawableSize = CGSizeMake(Width, Height);

		OctaneUI::Vector2 WindowSize = Window->GetSize();
		OctaneUI::Vector2 Scale((float)Width / WindowSize.X, (float)Height / WindowSize.Y);

		[Encoder setCullMode:MTLCullModeNone];
		[Encoder setDepthStencilState:g_DepthStencil];

		MTLViewport Viewport =
		{
			.originX = 0.0,
			.originY = 0.0,
			.width = (double)(WindowSize.X * Scale.X),
			.height = (double)(WindowSize.Y * Scale.Y),
			.znear = 0.0,
			.zfar = 1.0
		};
		[Encoder setViewport:Viewport];

		float PosX = 0.0f;
		float PosY = 0.0f;

		float L = PosX;
		float R = PosX + WindowSize.X;
		float T = PosY;
		float B = PosY + WindowSize.Y;
		float Near = Viewport.znear;
		float Far = Viewport.zfar;
		const float Ortho[4][4] =
		{
			{2.0f / (R-L), 0.0f, 0.0f, 0.0f},
			{0.0f, 2.0f / (T-B), 0.0f, 0.0f},
			{0.0f, 0.0f, 1/(Far - Near), 0.0f},
			{(R+L)/(L-R), (T+B)/(B-T), Near/(Far - Near), 1.0f}
		};
		[Encoder setVertexBytes:&Ortho length:sizeof(Ortho) atIndex:1];

		[Encoder setVertexBuffer:g_VertexBuffer offset:0 atIndex:0];
		[Encoder setVertexBufferOffset:0 atIndex:0];

		// TODO: Callback should give a buffer object that contains the full list of vertices and indices,
		// and a vector of draw commands to issue.
		std::vector<OctaneUI::Vertex> Vertices;
		std::vector<uint32_t> Indices;
		for (const OctaneUI::VertexBuffer& Item : Buffers)
		{
			Vertices.insert(Vertices.end(), Item.GetVertices().begin(), Item.GetVertices().end());
			Indices.insert(Indices.end(), Item.GetIndices().begin(), Item.GetIndices().end());
		}

		memcpy(g_VertexBuffer.contents, Vertices.data(), Vertices.size() * sizeof(OctaneUI::Vertex));
		memcpy(g_IndexBuffer.contents, Indices.data(), Indices.size() * sizeof(uint32_t));

		size_t VertexOffset = 0;
		size_t IndexOffset = 0;
		for (const OctaneUI::VertexBuffer& Item : Buffers)
		{
			// TODO: Investigate a way to have a single MTLRenderPipelineState.
			// These are split up into two states for how we sample a texture in the fragment
			// for one pipeline and only use the vertex color for the other.
			id<MTLTexture> Texture = GetTexture(Item.GetTextureID());
			if (Texture)
			{
				[Encoder setRenderPipelineState:g_RenderPipelineTextured];
				[Encoder setFragmentTexture:Texture atIndex:0];
			}
			else
			{
				[Encoder setRenderPipelineState:g_RenderPipeline];
			}

			[Encoder setVertexBufferOffset:VertexOffset atIndex:0];
			[Encoder
				drawIndexedPrimitives:MTLPrimitiveTypeTriangle
				indexCount:Item.GetIndices().size()
				indexType:MTLIndexTypeUInt32
				indexBuffer:g_IndexBuffer
				indexBufferOffset:IndexOffset
			];

			VertexOffset += Item.GetVertices().size() * sizeof(OctaneUI::Vertex);
			IndexOffset += Item.GetIndices().size() * sizeof(uint32_t);
		}

		[Encoder endEncoding];
		[Buffer presentDrawable:Surface];
		[Buffer commit];
	}
}

uint32_t OnLoadTexture(const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height)
{
	if (g_Device == nullptr)
	{
		return 0;
	}

	MTLTextureDescriptor* Descriptor = 
	[
		MTLTextureDescriptor
		texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
		width:(NSUInteger)Width
		height:(NSUInteger)Height
		mipmapped:NO
	];

	Descriptor.usage = MTLTextureUsageShaderRead;
	Descriptor.storageMode = MTLStorageModeManaged;
	id<MTLTexture> Texture = [g_Device newTextureWithDescriptor:Descriptor];
	[Texture replaceRegion:
		MTLRegionMake2D(0, 0, (NSUInteger)Width, (NSUInteger)Height)
		mipmapLevel:0
		withBytes:&Data[0]
		bytesPerRow:(NSUInteger)Width * 4
	];

	Textures.emplace_back(Texture);

	return Textures.back().ID;
}

void OnExit()
{
	Textures.clear();
	SDL_Quit();
}

void Initialize(OctaneUI::Application& Application)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		return;
	}

	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");

	Application.SetOnCreateWindow(OnCreateWindow);
	Application.SetOnDestroyWindow(OnDestroyWindow);
	Application.SetOnEvent(OnEvent);
	Application.SetOnPaint(OnPaint);
	Application.SetOnLoadTexture(OnLoadTexture);
	Application.SetOnExit(OnExit);
}

}
