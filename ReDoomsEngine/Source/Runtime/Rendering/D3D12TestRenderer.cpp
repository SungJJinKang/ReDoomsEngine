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
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset2)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset3)
		)
	)
);

void D3D12TestRenderer::Init()
{
	FRenderer::Init();

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	float m_aspectRatio = 1.0f;
	Vertex TriangleVertices[] =
	{
		{ { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};
	const size_t VerticeSize = sizeof(TriangleVertices);
	VerticeStride = sizeof(Vertex);

	VertexBuffer = eastl::make_unique<FD3D12BufferResource>(VerticeSize, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE, 0, false);
	VertexBuffer->InitResource();

	VertexBuffer->Map();
	EA::StdC::Memcpy(VertexBuffer->GetMappedAddress(), TriangleVertices, VerticeSize);
	VertexBuffer->Unmap();
}

void D3D12TestRenderer::OnStartFrame()
{
	FRenderer::OnStartFrame();

}

bool D3D12TestRenderer::Draw()
{
	FRenderer::Draw();

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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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

	FD3D12PSO* const PSO = FD3D12PSOManager::GetInstance()->GetOrCreatePSO(PSOInitializer);

	auto TestVSInstance = FTestVS::MakeShaderInstance();
	auto TestPSInstance = FTestPS::MakeShaderInstance();

	// Set necessary state.
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->SetPipelineState(PSO->PSOObject.Get());
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->SetGraphicsRootSignature(BoundShaderSet.GetRootSignature()->RootSignature.Get());

	TestVSInstance->Parameter.VertexOffset.MemberVariables.Offset = XMVECTOR{ 0.2f };
	TestVSInstance->Parameter.GlobalConstantBuffer.MemberVariables.AddOffset = true;
	TestVSInstance->Parameter.GlobalConstantBuffer.MemberVariables.ColorOffset1 = XMVECTOR{ 10.0f };
	TestVSInstance->Parameter.GlobalConstantBuffer.MemberVariables.ColorOffset2 = XMVECTOR{ 11.0f };

	TestPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.ColorOffset1 = XMVECTOR{ 12.0f };
	TestPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.ColorOffset2 = XMVECTOR{ 13.0f };
	TestPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.ColorOffset3 = XMVECTOR{ 14.0f };
	
	TestVSInstance->ApplyShaderParameter(CurrentFrameCommandContext, PSO->PSOInitializer.BoundShaderSet.GetRootSignature());
	TestPSInstance->ApplyShaderParameter(CurrentFrameCommandContext, PSO->PSOInitializer.BoundShaderSet.GetRootSignature());

	CD3DX12_VIEWPORT Viewport{ 0.0f, 0.0f, static_cast<float>(SwapChain->GetWidth()), static_cast<float>(SwapChain->GetHeight()) };
	CD3DX12_RECT Rect{ 0, 0, static_cast<LONG>(SwapChain->GetWidth()), static_cast<LONG>(SwapChain->GetHeight()) };
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetViewports(1, &Viewport);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetScissorRects(1, &Rect);

	// Indicate that the back buffer will be used as a render target.
	FD3D12RenderTargetResource& TargetRenderTarget = SwapChain->GetRenderTarget(SwapChain->GetCurrentBackbufferIndex());

	CD3DX12_RESOURCE_BARRIER ResourceBarrierA = CD3DX12_RESOURCE_BARRIER::Transition(TargetRenderTarget.GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ResourceBarrier(1, &ResourceBarrierA);

	CD3DX12_CPU_DESCRIPTOR_HANDLE RTVCPUHandle = TargetRenderTarget.GetRTV()->GetDescriptorHeapBlock().CPUDescriptorHandle();
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->OMSetRenderTargets(1, &RTVCPUHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ClearRenderTargetView(RTVCPUHandle, clearColor, 0, nullptr);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = VertexBuffer->GetVertexBufferView(VerticeStride);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->IASetVertexBuffers(0, 1, &VertexBufferView);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present.
	CD3DX12_RESOURCE_BARRIER ResourceBarrierB = CD3DX12_RESOURCE_BARRIER::Transition(TargetRenderTarget.GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ResourceBarrier(1, &ResourceBarrierB);

	if (!TestTexture)
	{
		TestTexture = FTextureLoader::LoadFromDDSFile(CurrentFrameCommandContext, EA_WCHAR("seafloor.dds"),
			D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT);

		TestTexture1 = FTextureLoader::LoadFromDDSFile(CurrentFrameCommandContext, EA_WCHAR("seafloor.dds"),
			D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT);

		FD3D12ResourceAllocator::GetInstance()->ResourceUploadBatcher.Flush(CurrentFrameCommandContext)->WaitOnLastSignal();
	}

	return true;
}

void D3D12TestRenderer::OnEndFrame()
{
	FRenderer::OnEndFrame();
}

void D3D12TestRenderer::Destroy()
{
	FRenderer::Destroy();

}
