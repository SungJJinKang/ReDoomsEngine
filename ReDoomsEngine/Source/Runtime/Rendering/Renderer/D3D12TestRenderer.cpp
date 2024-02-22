#include "D3D12TestRenderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "MeshLoader.h"

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
		ADD_SHADER_SRV_VARIABLE(TestTexture, EShaderParameterResourceType::Texture)
	)
);


DEFINE_SHADER(MeshDrawVS, "MeshDraw.hlsl", "MainVS", EShaderFrequency::Vertex, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Matrix, ModelMatrix)
		)
	)
);

DEFINE_SHADER(MeshDrawPS, "MeshDraw.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_SRV_VARIABLE(TriangleColorTexture, EShaderParameterResourceType::Texture)
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
		TestTexture = FTextureLoader::LoadFromFile(CurrentFrameCommandContext, EA_WCHAR("seafloor.dds")); // d3d debug layer doesn't complain even if don't transition to shader resource state. why????
		TestTexture->SetDebugNameToResource(EA_WCHAR("TestRenderer TestTexture"));

		SmallTexture = FTextureLoader::LoadFromFile(CurrentFrameCommandContext, EA_WCHAR("SmallTexture.dds"));
		SmallTexture->SetDebugNameToResource(EA_WCHAR("TestRenderer SmallTexture"));

	}

	if (!Mesh)
	{
		Mesh = FMeshLoader::LoadFromMeshFile(CurrentFrameCommandContext, EA_WCHAR("Drone/Drone.fbx"));
		//Mesh = FMeshLoader::LoadFromMeshFile(CurrentFrameCommandContext, EA_WCHAR("cabriolet-from-the-concept/source/Cabrio.fbx"));
		Mesh->Material[0].DiffuseTexture = FTextureLoader::LoadFromFile(CurrentFrameCommandContext, EA_WCHAR("Drone/Drone_diff.jpg"));
	}

	if (!VertexBuffer)
	{
		float m_aspectRatio = 1.0f;
		struct Vertex
		{
			Vector3 position;
			Vector4 color;
			Vector2 uv;
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

		eastl::vector<uint8> VertexData;
		VertexData.resize(sizeof(TriangleVertices));
		EA::StdC::Memcpy(VertexData.data(), TriangleVertices, sizeof(TriangleVertices));

		eastl::unique_ptr<FD3D12VertexIndexBufferSubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(eastl::move(VertexData));

		VertexBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(CurrentFrameCommandContext, eastl::move(SubresourceContainer), sizeof(Vertex));
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

	{
		auto TestVSInstance = FTestVS::MakeShaderInstance();
		auto TestPSInstance = FTestPS::MakeShaderInstance();

		eastl::array<eastl::shared_ptr<FD3D12ShaderInstance>, EShaderFrequency::NumShaderFrequency> ShaderList{};
		ShaderList[EShaderFrequency::Vertex] = TestVSInstance;
		ShaderList[EShaderFrequency::Pixel] = TestPSInstance;

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

		CurrentFrameCommandContext.DrawInstanced(3, 1, 0, 0);
		TestVSInstance->Parameter.GlobalConstantBuffer->ColorOffset2 = XMVECTOR{ 15.0f };

		XMVECTOR OriginalOffset = TestVSInstance->Parameter.VertexOffset->Offset;

		for (uint32_t i = 0; i < 100; ++i)
		{
			if (i >= 50)
			{
				TestPSInstance->Parameter.TestTexture = SmallTexture->GetSRV();
				TestVSInstance->Parameter.VertexOffset->Offset = OriginalOffset + XMVECTOR{ 0.0f, 0.5f, 0.0f, 0.0f } + XMVECTOR{ Offset, 0.0f, 0.0f, 0.0f } + XMVECTOR{ 0.1f, 0.0f, 0.0f, 0.0f } *(i - 50);
			}
			else
			{
				TestVSInstance->Parameter.VertexOffset->Offset = OriginalOffset + XMVECTOR{ Offset, 0.0f, 0.0f, 0.0f } + XMVECTOR{ 0.1f, 0.0f, 0.0f, 0.0f }*i;
			}
			CurrentFrameCommandContext.DrawInstanced(3, 1, 0, 0);
		}
	}

	{
		auto MeshDrawVSInstance = FMeshDrawVS::MakeShaderInstance();
		auto MeshDrawPSInstance = FMeshDrawPS::MakeShaderInstance();

		eastl::array<eastl::shared_ptr<FD3D12ShaderInstance>, EShaderFrequency::NumShaderFrequency> ShaderList{};
		ShaderList[EShaderFrequency::Vertex] = MeshDrawVSInstance;
		ShaderList[EShaderFrequency::Pixel] = MeshDrawPSInstance;

		FBoundShaderSet BoundShaderSet{ ShaderList };

		FD3D12PSOInitializer PSOInitializer{};
		PSOInitializer.BoundShaderSet = BoundShaderSet;
		PSOInitializer.Desc.InputLayout = { FMesh::InputElementDescs, _countof(FMesh::InputElementDescs) };
		PSOInitializer.Desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PSOInitializer.Desc.RasterizerState.FrontCounterClockwise = true;
		PSOInitializer.Desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PSOInitializer.Desc.DepthStencilState.DepthEnable = FALSE;
		PSOInitializer.Desc.DepthStencilState.StencilEnable = FALSE;
		PSOInitializer.Desc.SampleMask = UINT_MAX;
		PSOInitializer.Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PSOInitializer.Desc.NumRenderTargets = 1;
		PSOInitializer.Desc.RTVFormats[0] = SwapChain->GetFormat();
		PSOInitializer.Desc.SampleDesc.Count = 1;
		PSOInitializer.FinishCreating();

		// Set necessary state.
		CurrentFrameCommandContext.StateCache.SetPSO(PSOInitializer);

		float Speed = GTimeDelta * 10.0f;

		if (FD3D12Window::LeftArrowKeyPressed)
		{
			View.Transform.RotateYaw(Speed);
		}
		else if (FD3D12Window::RIghtArrowKeyPressed)
		{
			View.Transform.RotateYaw(-Speed);
		}
		
		if (FD3D12Window::UpArrowKeyPressed)
		{
			View.Transform.RotatePitch(Speed);
		}
		else if (FD3D12Window::DownArrowKeyPressed)
		{
			View.Transform.RotatePitch(-Speed);
		}

		if (FD3D12Window::WKeyPressed)
		{
			View.Transform.Translate(Vector3{0.0f, 0.0f, -1.0f} * Speed, ESpace::Self);
		}
		else if (FD3D12Window::SKeyPressed)
		{
			View.Transform.Translate(Vector3{ 0.0f, 0.0f, 1.0f } *Speed, ESpace::Self);
		}

		if (FD3D12Window::AKeyPressed)
		{
			View.Transform.Translate(Vector3{ -1.0f , 0.0f, 0.0f} *Speed, ESpace::Self);
		}
		else if (FD3D12Window::DKeyPressed)
		{
			View.Transform.Translate(Vector3{ 1.0f, 0.0f, 0.0f} *Speed, ESpace::Self);
		}
		 
		Matrix ModelMatrix = Matrix::CreateScale(0.05f, 0.05f, 0.05f) * Matrix::CreateRotationX(0) * Matrix::CreateTranslation(0.0f, 0.0f, -5.0f);

		Matrix ViewProjMat = View.GetViewPerspectiveProjectionMatrix(90.0f, SwapChain->GetWidth(), SwapChain->GetHeight());
		Matrix ViewMat = View.Get3DViewMatrices();
		Matrix ProjMat = View.GetPerspectiveProjectionMatrix(90.0f, SwapChain->GetWidth(), SwapChain->GetHeight());

		MeshDrawVSInstance->Parameter.GlobalConstantBuffer->ModelMatrix = ModelMatrix;
		MeshDrawVSInstance->Parameter.ViewConstantBuffer->ViewMatrix = ViewMat;
		MeshDrawVSInstance->Parameter.ViewConstantBuffer->ProjectionMatrix = ProjMat;
		MeshDrawVSInstance->Parameter.ViewConstantBuffer->ViewProjectionMatrix = ViewProjMat;
		MeshDrawPSInstance->Parameter.ViewConstantBuffer->ViewProjectionMatrix = ViewProjMat;
		MeshDrawPSInstance->Parameter.TriangleColorTexture = Mesh->Material[0].DiffuseTexture->GetSRV();


		eastl::vector<D3D12_VERTEX_BUFFER_VIEW> VertexBufferViewList = Mesh->MeshList[0].CreateVertexBufferViewList();
		CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->IASetVertexBuffers(0, VertexBufferViewList.size(), VertexBufferViewList.data());

		D3D12_INDEX_BUFFER_VIEW IndexBufferView = Mesh->MeshList[0].CreateIndexBufferView();
		CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->IASetIndexBuffer(&IndexBufferView);

		CurrentFrameCommandContext.DrawIndexedInstanced(Mesh->MeshList[0].IndexCount, 1, 0, 0, 0);
	}



	return true;
}

void D3D12TestRenderer::Destroy()
{
	FRenderer::Destroy();

}
