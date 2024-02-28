#include "D3D12TestRenderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "MeshLoader.h"
#include "RenderScene.h"

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
		ADD_SHADER_CONSTANT_BUFFER(MeshDrawConstantBuffer, MeshDrawConstantBuffer)
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

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
}


void D3D12TestRenderer::SceneSetup()
{
	FRenderer::SceneSetup();

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();

	TestTexture = FTextureLoader::LoadFromFile(CurrentFrameCommandContext, EA_WCHAR("seafloor.dds")); // d3d debug layer doesn't complain even if don't transition to shader resource state. why????
	TestTexture->SetDebugNameToResource(EA_WCHAR("TestRenderer TestTexture"));

	SmallTexture = FTextureLoader::LoadFromFile(CurrentFrameCommandContext, EA_WCHAR("SmallTexture.dds"));
	SmallTexture->SetDebugNameToResource(EA_WCHAR("TestRenderer SmallTexture"));

	
	DroneMesh = FMeshLoader::LoadFromMeshFile(CurrentFrameCommandContext, EA_WCHAR("Drone/Drone.fbx"));
	//Mesh = FMeshLoader::LoadFromMeshFile(CurrentFrameCommandContext, EA_WCHAR("cabriolet-from-the-concept/source/Cabrio.fbx"));
	DroneMesh->Material[0].DiffuseTexture = FTextureLoader::LoadFromFile(CurrentFrameCommandContext, EA_WCHAR("Drone/Drone_diff.jpg"));
	
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
		{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
		{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } }
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
	
	FD3D12PSOInitializer::FDrawDesc DroneDrawDesc;
	MEM_ZERO(DroneDrawDesc);

	D3D12_INPUT_LAYOUT_DESC InputDesc{ FMesh::InputElementDescs, _countof(FMesh::InputElementDescs) };
	CurrentFrameCommandContext.StateCache.SetPSOInputLayout(InputDesc);
	DroneDrawDesc.PSODesc.InputLayout = InputDesc;
	DroneDrawDesc.PSODesc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
	DroneDrawDesc.PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
	DroneDrawDesc.PSODesc.RasterizerState.FrontCounterClockwise = true;
	DroneDrawDesc.PSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
	DroneDrawDesc.PSODesc.DepthStencilState.DepthEnable = true;
	DroneDrawDesc.PSODesc.DepthStencilState.StencilEnable = false;
	DroneDrawDesc.PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	FMeshDrawArgument MeshDrawArgument;
	MeshDrawArgument.IndexCountPerInstance = DroneMesh->MeshList[0].IndexCount;
	MeshDrawArgument.InstanceCount = 1;
	MeshDrawArgument.StartIndexLocation = 0;
	MeshDrawArgument.BaseVertexLocation = 0;
	MeshDrawArgument.StartInstanceLocation = 0;

	for (int32_t IndexA = -20; IndexA < 20; ++IndexA)
	{
		for (int32_t IndexB = -20; IndexB < 20; ++IndexB)
		{
			auto MeshDrawVSInstance = MeshDrawVS.MakeTemplatedShaderInstance();
			auto MeshDrawPSInstance = MeshDrawPS.MakeTemplatedShaderInstance();

			MeshDrawPSInstance->Parameter.TriangleColorTexture = DroneMesh->Material[0].DiffuseTexture->GetSRV();

			eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency> ShaderList{};
			ShaderList[EShaderFrequency::Vertex] = MeshDrawVSInstance;
			ShaderList[EShaderFrequency::Pixel] = MeshDrawPSInstance;
			FBoundShaderSet BoundShaderSet{ ShaderList };
			DroneDrawDesc.BoundShaderSet = BoundShaderSet;

			Vector3 OrigianlPos{ 250.0f * IndexB, 250.0f * IndexA + 10.0f, -5.0f };
			FRenderObject RenderObject = RenderScene.AddRenderObject(
				true,
				DroneMesh->MeshList[0].AABB,
				OrigianlPos,
				Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(180), XMConvertToRadians(-90), 0.0f),
				Vector3{ 0.05f, 0.05f, 0.05f },
				2000.0f,
				DroneMesh->MeshList[0].VertexBufferViewList,
				DroneMesh->MeshList[0].IndexBufferView,
				DroneDrawDesc,
				MeshDrawArgument
			);
			DroneList.emplace_back(FDrone{ RenderObject,OrigianlPos });
		}
	}
}

void D3D12TestRenderer::OnStartFrame()
{
	FRenderer::OnStartFrame();

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();

	if (DepthStencilTarget == nullptr ||
		((DepthStencilTarget->GetDesc().Width != SwapChain->GetWidth()) || (DepthStencilTarget->GetDesc().Height != SwapChain->GetHeight()))
		)
	{
		DepthStencilTarget = FD3D12ResourceAllocator::GetInstance()->AllocateDepthTarget(SwapChain->GetWidth(), SwapChain->GetHeight());

		FD3D12PSOInitializer::FPassDesc BasePassPSODesc;
		MEM_ZERO(BasePassPSODesc);
		BasePassPSODesc.SampleMask = UINT_MAX;
		BasePassPSODesc.NumRenderTargets = 1;
		BasePassPSODesc.RTVFormats[0] = SwapChain->GetFormat();
		BasePassPSODesc.DSVFormat = DepthStencilTarget->GetDSV()->GetDesc()->Format;
		BasePassPSODesc.SampleDesc.Count = 1;

		RenderScene.PerPassData[static_cast<uint32_t>(EPass::BasePass)].PassPSODesc = BasePassPSODesc;
	}

	{
		float Speed = GTimeDelta * 3.0f;

		if (FD3D12Window::LeftArrowKeyPressed)
		{
			View.Transform.RotateYaw(Speed, ESpace::Self);
		}
		else if (FD3D12Window::RIghtArrowKeyPressed)
		{
			View.Transform.RotateYaw(-Speed, ESpace::Self);
		}

		if (FD3D12Window::UpArrowKeyPressed)
		{
			View.Transform.RotatePitch(Speed, ESpace::Self);
		}
		else if (FD3D12Window::DownArrowKeyPressed)
		{
			View.Transform.RotatePitch(-Speed, ESpace::Self);
		}

		if (FD3D12Window::WKeyPressed)
		{
			View.Transform.Translate(Vector3{ 0.0f, 0.0f, -1.0f } *Speed*50.0f, ESpace::Self);
		}
		else if (FD3D12Window::SKeyPressed)
		{
			View.Transform.Translate(Vector3{ 0.0f, 0.0f, 1.0f } *Speed * 50.0f, ESpace::Self);
		}

		if (FD3D12Window::AKeyPressed)
		{
			View.Transform.Translate(Vector3{ -1.0f , 0.0f, 0.0f } *Speed * 50.0f, ESpace::Self);
		}
		else if (FD3D12Window::DKeyPressed)
		{
			View.Transform.Translate(Vector3{ 1.0f, 0.0f, 0.0f } *Speed * 50.0f, ESpace::Self);
		}
		Matrix ViewProjMat = View.GetViewPerspectiveProjectionMatrix(90.0f, SwapChain->GetWidth(), SwapChain->GetHeight());
		Matrix ViewMat = View.Get3DViewMatrices();
		Matrix ProjMat = View.GetPerspectiveProjectionMatrix(90.0f, SwapChain->GetWidth(), SwapChain->GetHeight());
		ViewConstantBuffer.MemberVariables.ViewMatrix = ViewMat;
		ViewConstantBuffer.MemberVariables.ProjectionMatrix = ProjMat;
		ViewConstantBuffer.MemberVariables.ViewProjectionMatrix = ViewProjMat;
		ViewConstantBuffer.MemberVariables.ViewProjectionMatrix = ViewProjMat;
		ViewConstantBuffer.FlushShadowDataIfDirty();
	}
}

bool D3D12TestRenderer::Draw()
{
	FRenderer::Draw();

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();

	CD3DX12_VIEWPORT Viewport{ 0.0f, 0.0f, static_cast<float>(SwapChain->GetWidth()), static_cast<float>(SwapChain->GetHeight()) };
	CD3DX12_RECT Rect{ 0, 0, static_cast<LONG>(SwapChain->GetWidth()), static_cast<LONG>(SwapChain->GetHeight()) };

	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetViewports(1, &Viewport);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetScissorRects(1, &Rect);

	eastl::shared_ptr<FD3D12Texture2DResource>& RenderTarget = SwapChain->GetRenderTarget(GCurrentBackbufferIndex);

	// Indicate that the back buffer will be used as a render target.
	eastl::shared_ptr<FD3D12Texture2DResource>& TargetRenderTarget = SwapChain->GetRenderTarget(GCurrentBackbufferIndex);

	CD3DX12_RESOURCE_BARRIER ResourceBarrierA = CD3DX12_RESOURCE_BARRIER::Transition(TargetRenderTarget->GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ResourceBarrier(1, &ResourceBarrierA);

	CD3DX12_CPU_DESCRIPTOR_HANDLE RTVCPUHandle = TargetRenderTarget->GetRTV()->GetDescriptorHeapBlock().CPUDescriptorHandle();
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->OMSetRenderTargets(1, &RTVCPUHandle, FALSE, nullptr);

	CurrentFrameCommandContext.StateCache.SetRenderTargets({ RenderTarget.get() });
 	CurrentFrameCommandContext.StateCache.SetDepthEnable(true);
 	CurrentFrameCommandContext.StateCache.SetDepthStencilTarget(DepthStencilTarget.get());

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ClearRenderTargetView(RTVCPUHandle, clearColor, 0, nullptr);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ClearDepthStencilView(
		DepthStencilTarget->GetDSV()->GetDescriptorHeapBlock().CPUDescriptorHandle(),
		D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0.0f,
		0,
		nullptr
	);

	Offset += GTimeDelta;
	if (Offset > 5.0f)
	{
		Offset = -5.0f;
	}

	for (FDrone& Drone : DroneList)
	{
		Drone.RenderObject.SetPosition(Drone.OriginalPos + Vector3{ Offset * 80.0f, 0.0f, 0.0f });
	}

	RenderScene.PrepareToCreateMeshDrawList();

	// Base Pass
	{
		SCOPED_CPU_TIMER(Renderer_Draw_BasePass)

		eastl::vector<FMeshDraw> BasePassMeshDrawList = RenderScene.CreateMeshDrawListForPass(EPass::BasePass);

		{
			SCOPED_GPU_TIMER_DIRECT_QUEUE(CurrentFrameCommandContext, Renderer_BasePassDraw)
			for (FMeshDraw& MeshDraw : BasePassMeshDrawList)
			{
				MeshDraw.Draw(CurrentFrameCommandContext);
			}
		}
	}
	return true;
}

void D3D12TestRenderer::Destroy()
{
	FRenderer::Destroy();

}
