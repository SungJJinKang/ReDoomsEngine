#include "D3D12TestRenderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"

DEFINE_SHADER_CONSTANT_BUFFER_TYPE(
	VertexOffset,
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, Offset)
)

DEFINE_SHADER(TestVS, "Test/Test.hlsl", "VSMain", EShaderFrequency::Vertex, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(bool, AddOffset)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset1)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset2)
		)
		ADD_SHADER_CONSTANT_BUFFER(VertexOffset, VertexOffset)
	)
);

DEFINE_SHADER(TestPS, "Test/Test.hlsl", "PSMain", EShaderFrequency::Pixel, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset1)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset3)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset2)
		)
		ADD_SHADER_SRV_VARIABLE(TestTexture)
	)
);

void D3D12TestRenderer::Init()
{
	FRenderer::Init();
}

void D3D12TestRenderer::OnStartFrame()
{
	FRenderer::OnStartFrame();

	if (!TestTexture)
	{
		TestTexture = FTextureLoader::LoadFromDDSFile(CurrentFrameCommandContext, EA_WCHAR("seafloor.dds"),
			D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT, 
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE); // d3d debug layer doesn't complain even if don't transition to shader resource state. why????
		TestTexture->SetDebugNameToResource(EA_WCHAR("TestRenderer TestTexture"));

		SmallTexture = FTextureLoader::LoadFromDDSFile(CurrentFrameCommandContext, EA_WCHAR("SmallTexture.dds"),
			D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		SmallTexture->SetDebugNameToResource(EA_WCHAR("TestRenderer SmallTexture"));

	}

	if (!VertexBuffer)
	{
		float m_aspectRatio = 1.0f;
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
			XMFLOAT2 uv;
		};
		Vertex TriangleVertices[] =
		{
			{ { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.5f, 0.0f } },
			{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
			{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } }
		};

// 		Vertex TriangleVertices[] =
// 		{
// 			{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
// 			{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
// 			{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }
// 		};

		VertexBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(CurrentFrameCommandContext, reinterpret_cast<uint8_t*>(TriangleVertices), sizeof(TriangleVertices), sizeof(Vertex));
		VertexBuffer->SetDebugNameToResource(EA_WCHAR("TestRenderer VertexBuffer1"));
	}
}

bool D3D12TestRenderer::Draw()
{
	FRenderer::Draw();

	Offset += GTimeDelta;
	if (Offset > 2.0f)
	{
		Offset = -2.0f;
	}

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();

	//Test Code
	eastl::array<FD3D12ShaderTemplate*, EShaderFrequency::NumShaderFrequency> ShaderList{};
	ShaderList[EShaderFrequency::Vertex] = &TestVS;
	ShaderList[EShaderFrequency::Pixel] = &TestPS;

	FBoundShaderSet BoundShaderSet{ ShaderList };

	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	FD3D12PSOInitializer PSOInitializer{};
	PSOInitializer.BoundShaderSet = BoundShaderSet;
	PSOInitializer.Desc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	PSOInitializer.Desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	PSOInitializer.Desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PSOInitializer.Desc.DepthStencilState.DepthEnable = FALSE;
	PSOInitializer.Desc.DepthStencilState.StencilEnable = FALSE;
	PSOInitializer.Desc.SampleMask = UINT_MAX;
	PSOInitializer.Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PSOInitializer.Desc.NumRenderTargets = 1;
	PSOInitializer.Desc.RTVFormats[0] = SwapChain->GetFormat();
	PSOInitializer.Desc.SampleDesc.Count = 1;
	PSOInitializer.FinishCreating();

	auto TestVSInstance = FTestVS::MakeShaderInstance();
	auto TestPSInstance = FTestPS::MakeShaderInstance();

	// Set necessary state.
	CurrentFrameCommandContext.StateCache.SetPSO(PSOInitializer);

	TestVSInstance->Parameter.VertexOffset->Offset = XMVECTOR{ 0.4f };
	TestVSInstance->Parameter.GlobalConstantBuffer->AddOffset = true;
	TestVSInstance->Parameter.GlobalConstantBuffer->ColorOffset1 = XMVECTOR{ 10.0f };
	TestVSInstance->Parameter.GlobalConstantBuffer->ColorOffset2 = XMVECTOR{ 11.0f };

	TestPSInstance->Parameter.TestTexture = TestTexture->GetSRV();
	TestPSInstance->Parameter.GlobalConstantBuffer->ColorOffset1 = XMVECTOR{ 12.0f };
	TestPSInstance->Parameter.GlobalConstantBuffer->ColorOffset2 = XMVECTOR{ 13.0f };
	TestPSInstance->Parameter.GlobalConstantBuffer->ColorOffset3 = XMVECTOR{ 14.0f };
	
	TestVSInstance->ApplyShaderParameter(CurrentFrameCommandContext);
	TestPSInstance->ApplyShaderParameter(CurrentFrameCommandContext);

	CD3DX12_VIEWPORT Viewport{ 0.0f, 0.0f, static_cast<float>(SwapChain->GetWidth()), static_cast<float>(SwapChain->GetHeight()) };
	CD3DX12_RECT Rect{ 0, 0, static_cast<LONG>(SwapChain->GetWidth()), static_cast<LONG>(SwapChain->GetHeight()) };
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetViewports(1, &Viewport);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetScissorRects(1, &Rect);

	// Indicate that the back buffer will be used as a render target.
	eastl::shared_ptr<FD3D12RenderTargetResource>& TargetRenderTarget = SwapChain->GetRenderTarget(GCurrentBackbufferIndex);

	CD3DX12_RESOURCE_BARRIER ResourceBarrierA = CD3DX12_RESOURCE_BARRIER::Transition(TargetRenderTarget->GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ResourceBarrier(1, &ResourceBarrierA);

	CD3DX12_CPU_DESCRIPTOR_HANDLE RTVCPUHandle = TargetRenderTarget->GetRTV()->GetDescriptorHeapBlock().CPUDescriptorHandle();
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->OMSetRenderTargets(1, &RTVCPUHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ClearRenderTargetView(RTVCPUHandle, clearColor, 0, nullptr);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = VertexBuffer->GetVertexBufferView();
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->IASetVertexBuffers(0, 1, &VertexBufferView);

	CurrentFrameCommandContext.DrawInstanced(3, 1, 0, 0);
	CurrentFrameCommandContext.StateCache.SetPSO(PSOInitializer);

	TestVSInstance->Parameter.VertexOffset->Offset = XMVECTOR{ -0.4f };
	TestVSInstance->Parameter.GlobalConstantBuffer->ColorOffset2 = XMVECTOR{ 15.0f };
	TestVSInstance->ApplyShaderParameter(CurrentFrameCommandContext);

	CurrentFrameCommandContext.DrawInstanced(3, 1, 0, 0);
	TestVSInstance->Parameter.GlobalConstantBuffer->ColorOffset2 = XMVECTOR{ 15.0f };
	TestVSInstance->ApplyShaderParameter(CurrentFrameCommandContext);

	XMVECTOR OriginalOffset = TestVSInstance->Parameter.VertexOffset->Offset;

	for (uint32_t i = 0; i < 100; ++i)
	{
		if (i >= 50)
		{
			TestPSInstance->Parameter.TestTexture = SmallTexture->GetSRV();
			TestVSInstance->Parameter.VertexOffset->Offset = OriginalOffset + XMVECTOR{0.0f, 0.5f, 0.0f, 0.0f} + XMVECTOR{ Offset, 0.0f, 0.0f, 0.0f} + XMVECTOR{ 0.1f, 0.0f, 0.0f, 0.0f } * (i - 50);
		}
		else
		{
			TestVSInstance->Parameter.VertexOffset->Offset = OriginalOffset + XMVECTOR{ Offset, 0.0f, 0.0f, 0.0f } + XMVECTOR{ 0.1f, 0.0f, 0.0f, 0.0f }* i;
		}

		TestVSInstance->ApplyShaderParameter(CurrentFrameCommandContext);
		TestPSInstance->ApplyShaderParameter(CurrentFrameCommandContext);
		CurrentFrameCommandContext.DrawInstanced(3, 1, 0, 0);
	}

	return true;
}

void D3D12TestRenderer::Destroy()
{
	FRenderer::Destroy();

}
