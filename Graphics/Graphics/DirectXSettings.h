#pragma once
#include "LightConstantBuffer.h"
#include "WaterConstantBuffer.h"

struct SDirectXSettings
{
	static const int m_BackbufferAmount = 3;
	bool m_IsInitialized = false;

	ComPtr<ID3D12Device2> m_Device;
	ComPtr<ID3D12CommandQueue> m_DirectCommandQueue;
	ComPtr<ID3D12GraphicsCommandList2> m_DirectCommandList;

	ComPtr<IDXGISwapChain4> m_SwapChain;
	ComPtr<ID3D12Resource> m_BackbufferArray[m_BackbufferAmount];
	int m_BackbufferIndex;
	ComPtr<ID3D12CommandAllocator> m_Allocators[m_BackbufferAmount];
	float m_ClearColor[4] = { 0.1f, 0.1f, 0.7f };
	float m_BackbufferColors[m_BackbufferAmount][4] =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 1.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 1.0f, 0.0f}
	};
	ComPtr<ID3D12DescriptorHeap> m_RenderTargetHeap;

	ComPtr<ID3D12Resource> m_DepthBuffer;
	ComPtr<ID3D12DescriptorHeap> m_StencilHeap;

	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_ScissorRect;

	ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
	UINT m_DescriptorSize;

	ComPtr<ID3D12Fence> m_Fence;
	UINT64 m_FenceValue = 0;
	UINT64 m_FrameFenceValue[m_BackbufferAmount];
	HANDLE m_FrameEvent;

	XMMATRIX m_ViewMatrix;
	XMMATRIX m_ProjectionMatrix;

	ComPtr<ID3D12DescriptorHeap> m_ResourceDescriptors;
	UINT m_CBVDescriptorSize;

	// Simple Shader
	ComPtr<ID3D12PipelineState> m_SimpleShaderPipelineState;
	ComPtr<ID3D12RootSignature> m_SimpleShaderRootSignature;

	// Cell Shader
	ComPtr<ID3D12PipelineState> m_CellShadingPipelineState;
	ComPtr<ID3D12RootSignature> m_CellShadingRootSignature;

	// Textured Shader
	ComPtr<ID3D12PipelineState> m_TexturedShaderPipelineState;
	ComPtr<ID3D12RootSignature> m_TexturedShaderRootSignature;

	// HeightBlending Shader
	ComPtr<ID3D12PipelineState> m_HeightBlendingPipelineState;
	ComPtr<ID3D12RootSignature> m_HeightBlendingRootSignature;

	// Terrain Shader
	ComPtr<ID3D12PipelineState> m_TerrainShaderPipelineState;
	ComPtr<ID3D12RootSignature> m_TerrainShaderRootSignature;

	// Water Shader
	ComPtr<ID3D12PipelineState> m_WaterShaderPipelineState;
	ComPtr<ID3D12RootSignature> m_WaterShaderRootSignature;

	// LightConstantBuffer
	SLightConstantBuffer m_LightConstantBufferData;
	UINT8* m_LightConstantBufferDataBegin;
	ComPtr<ID3D12Resource> m_LightConstantBufferResource;
	D3D12_ROOT_DESCRIPTOR_TABLE1 m_LightConstantBufferDescriptor;

	// WaterConstantBuffer
	SWaterConstantBuffer m_WaterConstantBufferData;
	UINT8* m_WaterConstantBufferDataBegin;
	ComPtr<ID3D12Resource> m_WaterConstantBufferResource;
	D3D12_ROOT_DESCRIPTOR_TABLE1 m_WaterConstantBufferDescriptor;

	std::unique_ptr<SpriteBatch> m_SpriteBatch;
	std::unique_ptr<SpriteFont> m_SpriteFont;
	std::unique_ptr<GraphicsMemory> m_GraphicsMemory;
};

enum class EResourceType
{
	START_BUTTON,
	RETRY_BUTTON,
	NEXT_LEVEL_BUTTON,
	QUIT_BUTTON,
	WIN_IMAGE,
	LOSE_IMAGE,
	MENU_IMAGE,
	LIGHT_CONSTANT_BUFFER,
	WATER_CONSTANT_BUFFER,
	DICE_TEXTURE,
	QUAD_TEXTURE,
	BLUE_BUILDING,
	BRICK_BUILDING,
	BROWN_BUILDING,
	LIGHT_BROWN_BUILDING,
	GLASS_BUILDING,
	GRAY_BUILDING,
	MODERN_BUILDING,
	OLD_BUILDING,
	/*STONE_TEXTURE,
	STONE_HEIGHTMAP,
	SAND_TEXTURE,
	SAND_HEIGHTMAP,*/
	SPLATMAP_PIXEL,
	SPLATMAP_VERTEX,
	ATLAS_TEXTURE,
	SQUIRREL,
	WATER_HEIGTH,
	WATER_NORMAL,
	SPRITEFONT,
	TEST_IMAGE,
	CURSOR,
	MAX
};