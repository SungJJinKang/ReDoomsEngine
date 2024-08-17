#include "D3D12TestRenderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "MeshLoader.h"
#include "RenderScene.h"
#include "SceneData/GPUScene.h"

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
		ADD_SHADER_SRV_VARIABLE(GPrimitiveSceneData, EShaderParameterResourceType::StructuredBuffer)
	)
);

DEFINE_SHADER(MeshDrawPS, "MeshDraw.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_SRV_VARIABLE(BaseColor, EShaderParameterResourceType::Texture)
		ADD_SHADER_SRV_VARIABLE(Emissive, EShaderParameterResourceType::Texture)
		ADD_SHADER_SRV_VARIABLE(Metalic, EShaderParameterResourceType::Texture)
		ADD_SHADER_SRV_VARIABLE(Roughness, EShaderParameterResourceType::Texture)
		ADD_SHADER_SRV_VARIABLE(AmbientOcclusion, EShaderParameterResourceType::Texture)
	)
);

void D3D12TestRenderer::Init()
{
	FRenderer::Init();

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();

	CreateRenderTargets();
}

void D3D12TestRenderer::CreateRenderTargets()
{
	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();

	if (GBufferManager.GBufferA == nullptr ||
		((GBufferManager.GBufferA->GetDesc().Width != SwapChain->GetWidth()) || (GBufferManager.GBufferA->GetDesc().Height != SwapChain->GetHeight()))
		)
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		GBufferManager.GBufferA = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTarget(SwapChain->GetWidth(), SwapChain->GetHeight(), ClearColor);
		GBufferManager.GBufferB = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTarget(SwapChain->GetWidth(), SwapChain->GetHeight(), ClearColor);
		GBufferManager.GBufferC = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTarget(SwapChain->GetWidth(), SwapChain->GetHeight(), ClearColor);
		GBufferManager.GBufferD = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTarget(SwapChain->GetWidth(), SwapChain->GetHeight(), ClearColor);
		GBufferManager.Depth = FD3D12ResourceAllocator::GetInstance()->AllocateDepthStencilTarget(SwapChain->GetWidth(), SwapChain->GetHeight());
	}
	
	FD3D12PSOInitializer::FPassDesc BasePassPSODesc;
	MEM_ZERO(BasePassPSODesc);
	BasePassPSODesc.Desc.SampleMask = UINT_MAX;
	BasePassPSODesc.Desc.NumRenderTargets = 4;
	BasePassPSODesc.Desc.RTVFormats[0] = GBufferManager.GBufferA->GetDesc().Format;
	BasePassPSODesc.Desc.RTVFormats[1] = GBufferManager.GBufferB->GetDesc().Format;
	BasePassPSODesc.Desc.RTVFormats[2] = GBufferManager.GBufferC->GetDesc().Format;
	BasePassPSODesc.Desc.RTVFormats[3] = GBufferManager.GBufferD->GetDesc().Format;
	BasePassPSODesc.Desc.DSVFormat = GBufferManager.Depth->GetDSV()->GetDesc()->Format;
	BasePassPSODesc.Desc.SampleDesc.Count = 1;
	BasePassPSODesc.Desc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
	BasePassPSODesc.Desc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
	BasePassPSODesc.Desc.RasterizerState.FrontCounterClockwise = true;
	BasePassPSODesc.Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
	BasePassPSODesc.Desc.DepthStencilState.DepthEnable = true;
	BasePassPSODesc.Desc.DepthStencilState.StencilEnable = false;

	RenderScene.SetPassDesc(EPass::BasePass, BasePassPSODesc);
}

void D3D12TestRenderer::SceneSetup()
{
	FRenderer::SceneSetup();

	Level.UploadModel(CurrentFrameCommandContext, EA_WCHAR("Bistro/BistroExterior.fbx"));

	for (eastl::shared_ptr<FMeshModel>& Model : Level.ModelList)
	{
		FD3D12PSOInitializer::FDrawDesc DrawDesc;
		MEM_ZERO(DrawDesc);

		D3D12_INPUT_LAYOUT_DESC InputDesc{ FMesh::InputElementDescs, _countof(FMesh::InputElementDescs) };
		CurrentFrameCommandContext.StateCache.SetPSOInputLayout(InputDesc);
		DrawDesc.Desc.InputLayout = InputDesc;
		DrawDesc.Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		
		FMeshDrawArgument MeshDrawArgument;
		MeshDrawArgument.IndexCountPerInstance = Model->Mesh.IndexCount;
		MeshDrawArgument.InstanceCount = 1;
		MeshDrawArgument.StartIndexLocation = 0;
		MeshDrawArgument.BaseVertexLocation = 0;
		MeshDrawArgument.StartInstanceLocation = 0;
		
		auto MeshDrawVSInstance = MeshDrawVS.MakeTemplatedShaderInstance();
		auto MeshDrawPSInstance = MeshDrawPS.MakeTemplatedShaderInstance();

		MeshDrawPSInstance->Parameter.BaseColor = Model->Material.BaseColor ? Model->Material.BaseColor->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
		MeshDrawPSInstance->Parameter.Emissive = Model->Material.Emissive ? Model->Material.Emissive->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
		MeshDrawPSInstance->Parameter.Metalic = Model->Material.Metalic ? Model->Material.Metalic->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
		MeshDrawPSInstance->Parameter.Roughness = Model->Material.Roughness ? Model->Material.Roughness->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
		MeshDrawPSInstance->Parameter.AmbientOcclusion = Model->Material.AmbientOcclusion ? Model->Material.AmbientOcclusion->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();

		eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency> ShaderList{};
		ShaderList[EShaderFrequency::Vertex] = MeshDrawVSInstance;
		ShaderList[EShaderFrequency::Pixel] = MeshDrawPSInstance;
		FBoundShaderSet BoundShaderSet{ ShaderList };
		DrawDesc.BoundShaderSet = BoundShaderSet;

		FPrimitive Primitive = RenderScene.AddPrimitive(
			true,
			Model->Mesh.AABB,
			EPrimitiveFlag::CacheMeshDrawCommand | EPrimitiveFlag::AllowMergeMeshDrawCommand,
			Vector3{ 0.0f, 0.0f, 0.0f },
			Quaternion::Identity,
			Vector3{ 1.0f, 1.0f, 1.0f },
			2000.0f,
			Model->Mesh.VertexBufferViewList,
			Model->Mesh.IndexBufferView,
			DrawDesc,
			MeshDrawArgument
		);
	}
}

void D3D12TestRenderer::OnStartFrame()
{
	FRenderer::OnStartFrame();

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
	CreateRenderTargets();

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
			View.Transform.Translate(Vector3{ 0.0f, 0.0f, -1.0f } *Speed * 50.0f, ESpace::Self);
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
		ViewConstantBuffer.MemberVariables.PrevViewProjectionMatrix = ViewProjMat;
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

	CurrentFrameCommandContext.StateCache.SetRenderTargets(
		{ GBufferManager.GBufferA.get(), GBufferManager.GBufferB.get(), GBufferManager.GBufferC.get(), GBufferManager.GBufferD.get() }
	);
 	CurrentFrameCommandContext.StateCache.SetDepthEnable(true);
 	CurrentFrameCommandContext.StateCache.SetDepthStencilTarget(GBufferManager.Depth.get());
	GBufferManager.GBufferA->ClearRenderTargetView(CurrentFrameCommandContext);
	GBufferManager.GBufferB->ClearRenderTargetView(CurrentFrameCommandContext);
	GBufferManager.GBufferC->ClearRenderTargetView(CurrentFrameCommandContext);
	GBufferManager.GBufferD->ClearRenderTargetView(CurrentFrameCommandContext);
	GBufferManager.Depth->ClearDepthStencilView(CurrentFrameCommandContext);

	PrepareDraw(CurrentFrameCommandContext);

	// Base Pass
	{
		SCOPED_CPU_TIMER(Renderer_Draw_BasePass)

		eastl::vector<FMeshDraw> BasePassMeshDrawList = RenderScene.CreateMeshDrawListForPass(EPass::BasePass);

		{
			SCOPED_GPU_TIMER_DIRECT_QUEUE(CurrentFrameCommandContext, Renderer_BasePassDraw)
			for (FMeshDraw& MeshDraw : BasePassMeshDrawList)
			{
				MeshDraw.Draw(CurrentFrameCommandContext, *(RenderScene.GPUSceneData.GetPrimitiveIDBuffer()));
			}
		}
	}
	return true;
}

void D3D12TestRenderer::Destroy()
{
	FRenderer::Destroy();

}
