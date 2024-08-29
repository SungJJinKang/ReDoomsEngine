#include "D3D12TestRenderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "MeshLoader.h"
#include "RenderScene.h"
#include "SceneData/GPUScene.h"
#include "GlobalResources.h"
#include "Utils/ConsoleVariable.h"

static TConsoleVariable<Vector3> GDirectionalLightYawPitchRoll{ "r.DirectionalLightYawPitchRoll", Vector3{ 0.0f, 0.0f, -150.0f } };
static TConsoleVariable<Vector3> GDirectionLightColor{ "r.DirectionLightColor", Vector3{ 3.0f, 3.0f, 3.0f } };

DEFINE_SHADER(TestVS, "Test/Test.hlsl", "VSMain", EShaderFrequency::Vertex, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(bool, AddOffset)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, ColorOffset1)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, ColorOffset2)
		)
		ADD_SHADER_CONSTANT_BUFFER(VertexOffset, VertexOffset)
	)
);

DEFINE_SHADER(TestPS, "Test/Test.hlsl", "PSMain", EShaderFrequency::Pixel, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, ColorOffset1)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, ColorOffset3)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, ColorOffset2)
		)
		ADD_SHADER_SRV_VARIABLE(TestTexture, EShaderParameterResourceType::Texture)
	)
);

DEFINE_SHADER(MeshDrawVS, "MeshDrawVS.hlsl", "MeshDrawVS", EShaderFrequency::Vertex, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_SRV_VARIABLE(GPrimitiveSceneData, EShaderParameterResourceType::StructuredBuffer)
	)
);

DEFINE_SHADER(SponzaMeshDrawPS, "SponzaMeshDrawPS.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(DiffuseTexture, EShaderParameterResourceType::Texture)
		ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(NormalTexture, EShaderParameterResourceType::Texture)
		ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(EmissiveTexture, EShaderParameterResourceType::Texture)
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(float, Metalic)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(float, Roughness)
		)
	)
);


DEFINE_SHADER(ScreenDrawVS, "ScreenDrawVS.hlsl", "ScreenDrawVS", EShaderFrequency::Vertex, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, PosScaleUVScale)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, InvTargetSizeAndTextureSize)
		)
	)
);

DEFINE_SHADER(DeferredShadingPS, "DeferredShadingPS.hlsl", "DeferredShadingPS", EShaderFrequency::Pixel, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_GBUFFER_SHADER_SRV()
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector3, LightDirection)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector3, LightColor)
		)
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
		GBufferManager.GBufferA->SetDebugNameToResource(EA_WCHAR("GBufferA"));
		GBufferManager.GBufferB = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTarget(SwapChain->GetWidth(), SwapChain->GetHeight(), ClearColor);
		GBufferManager.GBufferB->SetDebugNameToResource(EA_WCHAR("GBufferB"));
		GBufferManager.GBufferC = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTarget(SwapChain->GetWidth(), SwapChain->GetHeight(), ClearColor);
		GBufferManager.GBufferC->SetDebugNameToResource(EA_WCHAR("GBufferC"));
		GBufferManager.Depth = FD3D12ResourceAllocator::GetInstance()->AllocateDepthStencilTarget(SwapChain->GetWidth(), SwapChain->GetHeight());
		GBufferManager.Depth->SetDebugNameToResource(EA_WCHAR("Depth"));
	}
	
	FD3D12PSOInitializer::FPassDesc BasePassPSODesc{};
	BasePassPSODesc.Desc.SampleMask = UINT_MAX;
	BasePassPSODesc.Desc.NumRenderTargets = 4;
	BasePassPSODesc.Desc.RTVFormats[0] = GBufferManager.GBufferA->GetDesc().Format;
	BasePassPSODesc.Desc.RTVFormats[1] = GBufferManager.GBufferB->GetDesc().Format;
	BasePassPSODesc.Desc.RTVFormats[2] = GBufferManager.GBufferC->GetDesc().Format;
	BasePassPSODesc.Desc.DSVFormat = GBufferManager.Depth->GetDSV()->GetDesc()->Format;
	BasePassPSODesc.Desc.SampleDesc.Count = 1;
	BasePassPSODesc.Desc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
	BasePassPSODesc.Desc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
	BasePassPSODesc.Desc.RasterizerState.FrontCounterClockwise = true;
	BasePassPSODesc.Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
	BasePassPSODesc.Desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
	BasePassPSODesc.Desc.DepthStencilState.DepthEnable = true;
	BasePassPSODesc.Desc.DepthStencilState.StencilEnable = false;

	RenderScene.SetPassDesc(EPass::BasePass, BasePassPSODesc);
}

void D3D12TestRenderer::SceneSetup()
{
	FRenderer::SceneSetup();

	{
		Level.UploadModel(CurrentFrameCommandContext, EA_WCHAR("DamagedHelmet/DamagedHelmet.gltf"), EMeshLoadFlags::SubstractOneFromV);
		//Level.UploadModel(CurrentFrameCommandContext, EA_WCHAR("Bistro/BistroExterior.fbx"));
		//Level.UploadModel(CurrentFrameCommandContext, EA_WCHAR("Bistro/BistroInterior.fbx"));
		for (FMeshModel& Model : Level.ModelList)
		{
			if (Model.Material->MaterialName.find("Metal") != Model.Material->MaterialName.npos)
			{
				// Bistro.fbx doesn't contains metal texture and factor.
				// So if material name contains "Metal", we set metalic factor to 1.0f.
				Model.Material->ConstantMetalicFactor = 1.0f;
			}
		}
	}

	for (FMeshModel& Model : Level.ModelList)
	{
		FD3D12PSOInitializer::FDrawDesc DrawDesc{};

		D3D12_INPUT_LAYOUT_DESC InputDesc{ FMesh::InputElementDescs, _countof(FMesh::InputElementDescs) };
		CurrentFrameCommandContext.StateCache.SetPSOInputLayout(InputDesc);
		DrawDesc.Desc.InputLayout = InputDesc;
		DrawDesc.Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		
		FMeshDrawArgument MeshDrawArgument;
		MeshDrawArgument.IndexCountPerInstance = Model.Mesh->IndexCount;
		MeshDrawArgument.InstanceCount = 1;
		MeshDrawArgument.StartIndexLocation = 0;
		MeshDrawArgument.BaseVertexLocation = 0;
		MeshDrawArgument.StartInstanceLocation = 0;
		
		// @TODO : Pool shader instance
		auto MeshDrawVSInstance = MeshDrawVS.MakeTemplatedShaderInstance();
		auto MeshDrawPSInstance = SponzaMeshDrawPS.MakeTemplatedShaderInstance();

		MeshDrawPSInstance->Parameter.DiffuseTexture = Model.Material->DiffuseTexture ? Model.Material->DiffuseTexture->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
		MeshDrawPSInstance->Parameter.NormalTexture = Model.Material->NormalsTexture ? Model.Material->NormalsTexture->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
		MeshDrawPSInstance->Parameter.EmissiveTexture = Model.Material->EmissionColorTexture ? Model.Material->EmissionColorTexture->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
		MeshDrawPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.Metalic
			= Model.Material->ConstantMetalicFactor;
		MeshDrawPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.Roughness
			= Model.Material->ConstantRoughnessFactor;

		eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency> ShaderList{};
		ShaderList[EShaderFrequency::Vertex] = MeshDrawVSInstance;
		ShaderList[EShaderFrequency::Pixel] = MeshDrawPSInstance;
		FBoundShaderSet BoundShaderSet{ ShaderList };
		DrawDesc.BoundShaderSet = BoundShaderSet;

		for(uint32_t InstanceIndex = 0; InstanceIndex < Model.InstanceLocalToWorldMatrixList.size(); ++InstanceIndex)
		{
			const Matrix LocalToWorldMatrix = Matrix::CreateTranslation(
				0.0f,
				0.0f,
				0.0f);

			const Matrix RotationMatrix = Matrix::CreateFromQuaternion(Quaternion::Identity);
			const Matrix ScaleMatrix = Matrix::CreateScale(1.0f, 1.0f, 1.0f);

			//
			RenderScene.AddPrimitive(
				true,
				Model.Mesh->AABB,
				EPrimitiveFlag::CacheMeshDrawCommand | EPrimitiveFlag::AllowMergeMeshDrawCommand,
				//Model.InstanceLocalToWorldMatrixList[InstanceIndex], @ TODO : Bistro asset has wrong matrix data, so we use this temporally
				LocalToWorldMatrix * RotationMatrix * ScaleMatrix,
				2000.0f,
				Model.Mesh->VertexBufferViewList,
				Model.Mesh->IndexBufferView,
				DrawDesc,
				MeshDrawArgument
			);
		}
	}
}

void D3D12TestRenderer::OnStartFrame()
{
	FRenderer::OnStartFrame();

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
	CreateRenderTargets();

	{
		float Speed = GTimeDelta * 1.0f;

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
		ViewConstantBuffer.MemberVariables.ViewWorldPosition = Vector4{ View.Transform.Position.x, View.Transform.Position.y, View.Transform.Position.z, 0.0f };
		ViewConstantBuffer.MemberVariables.ViewMatrix = ViewMat;
		ViewConstantBuffer.MemberVariables.InvViewProjectionMatrix = ViewMat.Invert();
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
		{ GBufferManager.GBufferA.get(), GBufferManager.GBufferB.get(), GBufferManager.GBufferC.get() }
	);
 	CurrentFrameCommandContext.StateCache.SetDepthEnable(true);
 	CurrentFrameCommandContext.StateCache.SetDepthStencilTarget(GBufferManager.Depth.get());
	GBufferManager.GBufferA->ClearRenderTargetView(CurrentFrameCommandContext);
	GBufferManager.GBufferB->ClearRenderTargetView(CurrentFrameCommandContext);
	GBufferManager.GBufferC->ClearRenderTargetView(CurrentFrameCommandContext);
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

	{
		eastl::shared_ptr<FD3D12Texture2DResource>& SwapChainRenderTarget = SwapChain->GetRenderTarget(GCurrentBackbufferIndex);
		CD3DX12_CPU_DESCRIPTOR_HANDLE RTVCPUHandle = SwapChainRenderTarget->GetRTV()->GetDescriptorHeapBlock().CPUDescriptorHandle();
		CurrentFrameCommandContext.StateCache.SetRenderTargets({ SwapChainRenderTarget.get() });
		CurrentFrameCommandContext.StateCache.SetDepthStencilTarget(nullptr);
		
		eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> VertexBufferViewList{};
		VertexBufferViewList.emplace_back(GScreenDrawPositionBuffer->GetVertexBufferView());
		VertexBufferViewList.emplace_back(GScreenDrawUVBuffer->GetVertexBufferView());

		CurrentFrameCommandContext.StateCache.SetVertexBufferViewList(VertexBufferViewList);
		CurrentFrameCommandContext.StateCache.SetIndexBufferView(GScreenDrawIndexBuffer->GetIndexBufferView());

		FD3D12PSOInitializer::FDrawDesc DrawDesc;

		static const D3D12_INPUT_ELEMENT_DESC ScreenDrawInputElementDescs[]{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_INPUT_LAYOUT_DESC InputDesc{ ScreenDrawInputElementDescs, _countof(ScreenDrawInputElementDescs) };
		DrawDesc.Desc.InputLayout = InputDesc;
		DrawDesc.Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		FMeshDrawArgument MeshDrawArgument;
		MeshDrawArgument.IndexCountPerInstance = 6;
		MeshDrawArgument.InstanceCount = 1;
		MeshDrawArgument.StartIndexLocation = 0;
		MeshDrawArgument.BaseVertexLocation = 0;
		MeshDrawArgument.StartInstanceLocation = 0;

		CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(GBufferManager.GBufferA->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(GBufferManager.GBufferB->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(GBufferManager.GBufferC->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(GBufferManager.Depth->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		auto ScreenDrawVSInstance = ScreenDrawVS.MakeTemplatedShaderInstance();
		auto DeferredShadingPSInstance = DeferredShadingPS.MakeTemplatedShaderInstance();

		ScreenDrawVSInstance->Parameter.GlobalConstantBuffer.MemberVariables.PosScaleUVScale
			= Vector4{ static_cast<float>(SwapChain->GetWidth()), static_cast<float>(SwapChain->GetHeight()), static_cast<float>(SwapChain->GetWidth()), static_cast<float>(SwapChain->GetHeight()) };

		ScreenDrawVSInstance->Parameter.GlobalConstantBuffer.MemberVariables.InvTargetSizeAndTextureSize 
			= Vector4{ 1.0f / SwapChain->GetWidth(), 1.0f / SwapChain->GetHeight(), 1.0f / static_cast<float>(GBufferManager.GBufferA->GetDesc().Width), 1.0f / static_cast<float>(GBufferManager.GBufferA->GetDesc().Height) };
		DeferredShadingPSInstance->Parameter.GBufferATexture = GBufferManager.GBufferA->GetTextureSRV();
		DeferredShadingPSInstance->Parameter.GBufferBTexture = GBufferManager.GBufferB->GetTextureSRV();
		DeferredShadingPSInstance->Parameter.GBufferCTexture = GBufferManager.GBufferC->GetTextureSRV();
		
		FD3D12SRVDesc SRVDesc{};
		SRVDesc.ShaderParameterResourceType = EShaderParameterResourceType::Texture;
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc{};
		MEM_ZERO(Desc);
		Desc.Format = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		Desc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
		Desc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0,0,0,0);
		Desc.Texture2D.MostDetailedMip = 0;
		Desc.Texture2D.MipLevels = -1;
		Desc.Texture2D.PlaneSlice = 0;
		Desc.Texture2D.ResourceMinLODClamp = 0.0f;
		SRVDesc.Desc = Desc;
		DeferredShadingPSInstance->Parameter.DepthTexture = GBufferManager.Depth->GetSRV(SRVDesc);

		Vector3 DirectionalLightYawPitchRoll = GDirectionalLightYawPitchRoll * DEGREE_TO_RADIAN;
		Vector3 LightDirection = Quaternion::CreateFromYawPitchRoll(DirectionalLightYawPitchRoll.x, DirectionalLightYawPitchRoll.y, DirectionalLightYawPitchRoll.z) * Vector3::Forward;
		
		DeferredShadingPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.LightDirection = LightDirection;
		DeferredShadingPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.LightColor = GDirectionLightColor;

		eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency> ShaderList{};
		ShaderList[EShaderFrequency::Vertex] = ScreenDrawVSInstance;
		ShaderList[EShaderFrequency::Pixel] = DeferredShadingPSInstance;
		FBoundShaderSet BoundShaderSet{ ShaderList };
		DrawDesc.BoundShaderSet = BoundShaderSet;
		CurrentFrameCommandContext.StateCache.SetPSODrawDesc(DrawDesc);

		FD3D12PSOInitializer::FPassDesc BasePassPSODesc{};
		BasePassPSODesc.Desc.SampleMask = UINT_MAX;
		BasePassPSODesc.Desc.NumRenderTargets = 1;
		BasePassPSODesc.Desc.RTVFormats[0] = SwapChainRenderTarget->GetDesc().Format;
		BasePassPSODesc.Desc.SampleDesc.Count = 1;
		BasePassPSODesc.Desc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
		BasePassPSODesc.Desc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
		BasePassPSODesc.Desc.RasterizerState.FrontCounterClockwise = true;
		BasePassPSODesc.Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
		BasePassPSODesc.Desc.DepthStencilState.DepthEnable = true;
		BasePassPSODesc.Desc.DepthStencilState.StencilEnable = false;

		CurrentFrameCommandContext.StateCache.SetPSOPassDesc(BasePassPSODesc);
		CurrentFrameCommandContext.StateCache.SetDepthEnable(false);

		CurrentFrameCommandContext.DrawIndexedInstanced(
			MeshDrawArgument.IndexCountPerInstance,
			MeshDrawArgument.InstanceCount,
			MeshDrawArgument.StartIndexLocation,
			MeshDrawArgument.BaseVertexLocation,
			MeshDrawArgument.StartInstanceLocation
		);

		CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(GBufferManager.GBufferA->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(GBufferManager.GBufferB->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(GBufferManager.GBufferC->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(GBufferManager.Depth->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	}


	return true;
}

void D3D12TestRenderer::Destroy()
{
	FRenderer::Destroy();

}
