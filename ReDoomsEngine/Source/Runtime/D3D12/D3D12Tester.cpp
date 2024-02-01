#include "D3D12Tester.h"

#include "D3D12RendererInclude.h"

// DEFINE_SHADER(HelloTriangleVS, "HelloTriangle.hlsl", "MainVS", EShaderFrequency::Vertex, EShaderCompileFlag::None,
// 	ADD_CONSTANT_BUFFER(SceneConstantBuffer,
// 		ADD_SHADER_GLOBAL_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, offset)
// 	)
// 	ADD_SHADER_GLOBAL_CONSTANT_BUFFER_MEMBER_VARIABLE(int, Time)
// 	ADD_PREPROCESSOR_DEFINE(NO_ERROR=1)
// 	ADD_PREPROCESSOR_DEFINE(NO_ERROR1=1)
// );
// DEFINE_SHADER(HelloTrianglePS, "HelloTriangle.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None, ADD_PREPROCESSOR_DEFINE(NO_ERROR=1));
// DEFINE_SHADER(HelloTrianglePS2, "HelloTriangle.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None, ADD_PREPROCESSOR_DEFINE(NO_ERROR=1));

DEFINE_SHADER(TestVS, "Test/Test.hlsl", "VSMain", EShaderFrequency::Vertex, EShaderCompileFlag::None, 
	DEFINE_SHADER_PARAMTERS(
		DEFINE_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_GLOBAL_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset1)
			ADD_SHADER_GLOBAL_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset2)
		)
	)
);

DEFINE_SHADER(TestPS, "Test/Test.hlsl", "PSMain", EShaderFrequency::Pixel, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		DEFINE_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_GLOBAL_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset1)
			ADD_SHADER_GLOBAL_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, ColorOffset2)
		)
	)
);

void D3D12Tester::Test()
{
	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();

	FD3D12CommandAllocator* const CommandAllocator = FD3D12CommandListManager::GetInstance()->GetOrCreateNewCommandAllocator(ED3D12QueueType::Direct);
	FD3D12CommandList* const CommandList = CommandAllocator->GetOrCreateNewCommandList();

	//Test Code
	eastl::array<FD3D12ShaderTemplate*, EShaderFrequency::NumShaderFrequency> ShaderList{};
	ShaderList[EShaderFrequency::Vertex] = &TestVS;
	ShaderList[EShaderFrequency::Pixel] = &TestPS;

	auto TestVSInstance = FTestPS::MakeShaderInstance();

	TestVS.ShaderParameter.GlobalConstantBuffer.ColorOffset1 = XMVECTOR{4};
	TestVS.ShaderParameter.GlobalConstantBuffer.ColorOffset2 = XMVECTOR{7};

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
	const size_t VerticeStride = sizeof(Vertex);

	FD3D12BufferResource VertexBuffer{ VerticeSize, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE, 0, false };
	VertexBuffer.InitResource();

	VertexBuffer.Map();
	EA::StdC::Memcpy(VertexBuffer.GetMappedAddress(), TriangleVertices, VerticeSize);
	VertexBuffer.Unmap();

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = VertexBuffer.GetVertexBufferView(VerticeStride);

	FD3D12CommandQueue* const TargetCommandQueue = FD3D12Device::GetInstance()->GetCommandQueue(ED3D12QueueType::Direct);

	while (true)
	{
		CommandAllocator->ResetCommandAllocator(false);

		CommandList->ResetRecordingCommandList(PSO);

		// Set necessary state.
		CommandList->GetD3DCommandList()->SetGraphicsRootSignature(FD3D12RootSignatureManager::GetInstance()->GetOrCreateRootSignature(BoundShaderSet)->RootSignature.Get());

		CD3DX12_VIEWPORT Viewport{ 0.0f, 0.0f, static_cast<float>(SwapChain->GetWidth()), static_cast<float>(SwapChain->GetHeight()) };
		CD3DX12_RECT Rect{ 0, 0, static_cast<LONG>(SwapChain->GetWidth()), static_cast<LONG>(SwapChain->GetHeight()) };
		CommandList->GetD3DCommandList()->RSSetViewports(1, &Viewport);
		CommandList->GetD3DCommandList()->RSSetScissorRects(1, &Rect);

		// Indicate that the back buffer will be used as a render target.
		FD3D12RenderTargetResource& TargetRenderTarget = SwapChain->GetRenderTarget(SwapChain->GetCurrentBackbufferIndex());

		CD3DX12_RESOURCE_BARRIER ResourceBarrierA = CD3DX12_RESOURCE_BARRIER::Transition(TargetRenderTarget.GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		CommandList->GetD3DCommandList()->ResourceBarrier(1, &ResourceBarrierA);

		CD3DX12_CPU_DESCRIPTOR_HANDLE RTVCPUHandle = TargetRenderTarget.GetRTV()->GetDescriptorHeapBlock().CPUDescriptorHandle();
		CommandList->GetD3DCommandList()->OMSetRenderTargets(1, &RTVCPUHandle, FALSE, nullptr);

		// Record commands.
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		CommandList->GetD3DCommandList()->ClearRenderTargetView(RTVCPUHandle, clearColor, 0, nullptr);
		CommandList->GetD3DCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CommandList->GetD3DCommandList()->IASetVertexBuffers(0, 1, &VertexBufferView);
		CommandList->GetD3DCommandList()->DrawInstanced(3, 1, 0, 0);

		// Indicate that the back buffer will now be used to present.
		CD3DX12_RESOURCE_BARRIER ResourceBarrierB = CD3DX12_RESOURCE_BARRIER::Transition(TargetRenderTarget.GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		CommandList->GetD3DCommandList()->ResourceBarrier(1, &ResourceBarrierB);

		CommandList->FinishRecordingCommandList(TargetCommandQueue);

		ID3D12CommandList* CommandLists[] = { CommandList->GetD3DCommandList() };
		TargetCommandQueue->GetD3DCommandQueue()->ExecuteCommandLists(_countof(CommandLists), CommandLists);

		SwapChain->Present(1);

		TargetCommandQueue->WaitForCompletion();

		SwapChain->UpdateCurrentBackbufferIndex();
	}
}
