#include "D3D12TestRenderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "MeshLoader.h"
#include "RenderScene.h"
#include "SceneData/GPUScene.h"
#include "GlobalResources.h"
#include "Utils/ConsoleVariable.h"

static TConsoleVariable<Vector3> GDirectionalLightYawPitchRoll{ "r.DirectionalLightYawPitchRoll", Vector3{ 250.0f, 1.0f, 75.0f } };
static TConsoleVariable<Vector3> GDirectionLightColor{ "r.DirectionLightColor", Vector3{ 3.0f, 3.0f, 3.0f } };
static TConsoleVariable<int32> GCubemapSize{ "r.CubemapSize", 512 };

static const D3D12_INPUT_ELEMENT_DESC ScreenDrawInputElementDescs[]{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

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

DEFINE_SHADER(SponzaMeshDrawMirrorSamplerPS, "SponzaMeshDrawPS.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(DiffuseTexture, EShaderParameterResourceType::Texture)
		ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(NormalTexture, EShaderParameterResourceType::Texture)
		ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(EmissiveTexture, EShaderParameterResourceType::Texture)
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(float, Metalic)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(float, Roughness)
		)
	)
	ADD_PREPROCESSOR_DEFINE(MIRROR_SAMPLER=1)
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
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, PosScaleUVScale)
		)
	)
);

// 
// DEFINE_SHADER(SetupEnvCubemapPS, "SetupEnvCubemapPS.hlsl", "SetupEnvCubemapPS", EShaderFrequency::Pixel, EShaderCompileFlag::None,
// 	DEFINE_SHADER_PARAMTERS(
// 		ADD_SHADER_SRV_VARIABLE(HDREnvMapTexture, EShaderParameterResourceType::Texture)
// 		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
// 			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Matrix, ViewMatrixForCubemap)
// 		)
// 	)
// );

DEFINE_SHADER(RenderCubemapVS, "RenderCubemap.hlsl", "RenderCubemapVS", EShaderFrequency::Vertex, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, PosScaleUVScale)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, InvTargetSizeAndTextureSize)
		)
	)
);
DEFINE_SHADER(RenderCubemapPS, "RenderCubemap.hlsl", "RenderCubemapPS", EShaderFrequency::Pixel, EShaderCompileFlag::None,
	DEFINE_SHADER_PARAMTERS(
		ADD_SHADER_SRV_VARIABLE(HDREnvMapTexture, EShaderParameterResourceType::Texture)
		ADD_SHADER_GLOBAL_CONSTANT_BUFFER(
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector2, CubemapSize)
			ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(int32, CubemapFaceIndex)
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
		GBufferManager.GBufferA = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTarget2D(SwapChain->GetWidth(), SwapChain->GetHeight(), ClearColor);
		GBufferManager.GBufferA->SetDebugNameToResource(EA_WCHAR("GBufferA"));
		GBufferManager.GBufferB = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTarget2D(SwapChain->GetWidth(), SwapChain->GetHeight(), ClearColor);
		GBufferManager.GBufferB->SetDebugNameToResource(EA_WCHAR("GBufferB"));
		GBufferManager.GBufferC = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTarget2D(SwapChain->GetWidth(), SwapChain->GetHeight(), ClearColor);
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
		FMeshModelCustomData MeshModelCustomDataForHelmet{};
		MeshModelCustomDataForHelmet.Transform.Position = Vector3{ 0.0f, 0.0f, 0.0f };
		MeshModelCustomDataForHelmet.Transform.Rotate({ -270.0f, 0.0, 0.0f });
		Level.UploadModel(
			CurrentFrameCommandContext, 
			EA_WCHAR("DamagedHelmet/DamagedHelmet.gltf"), 
			MeshModelCustomDataForHelmet,
			EMeshLoadFlags::SubstractOneFromV
		);
		for (FMeshModel& Model : Level.ModelList)
		{
			Model.Material->ConstantMetalicFactor = 1.0f;
		}

		HDREnvMapTexture = FTextureLoader::LoadTexture2DFromFile(CurrentFrameCommandContext,
			EA_WCHAR("Bistro/san_giuseppe_bridge_4k.hdr"),
			D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE,
			DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
		);

		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		EnvCubemap = FD3D12ResourceAllocator::GetInstance()->AllocateRenderTargetCube(GCubemapSize, GCubemapSize, ClearColor);
		EnvCubemap->SetDebugNameToResource(EA_WCHAR("EnvironmentMap"));

		//Level.UploadModel(CurrentFrameCommandContext, EA_WCHAR("Bistro/BistroExterior.fbx"), {}, EMeshLoadFlags::MirrorAddressModeIfTextureCoordinatesOutOfRange);
		//Level.UploadModel(CurrentFrameCommandContext, EA_WCHAR("Bistro/BistroInterior.fbx"{}, EMeshLoadFlags::MirrorAddressModeIfTextureCoordinatesOutOfRange);
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
		auto MeshDrawVSInstance = MeshDrawVS.MakeTemplatedMaterial();
		FD3D12Material* MeshDrawPSInstance = nullptr;

		// @TODO : Support shader permutation
		if (Model.Material->TextureMapMode[0] == ETextureMapMode::Mirror)
		{
			auto SponzaMeshDrawMirrorSamplerPSInstance = SponzaMeshDrawMirrorSamplerPS.MakeTemplatedMaterial();

			SponzaMeshDrawMirrorSamplerPSInstance->Parameter.DiffuseTexture = Model.Material->DiffuseTexture ? Model.Material->DiffuseTexture->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
			SponzaMeshDrawMirrorSamplerPSInstance->Parameter.NormalTexture = Model.Material->NormalsTexture ? Model.Material->NormalsTexture->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
			SponzaMeshDrawMirrorSamplerPSInstance->Parameter.EmissiveTexture = Model.Material->EmissionColorTexture ? Model.Material->EmissionColorTexture->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
			SponzaMeshDrawMirrorSamplerPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.Metalic
				= Model.Material->ConstantMetalicFactor;
			SponzaMeshDrawMirrorSamplerPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.Roughness
				= Model.Material->ConstantRoughnessFactor;
			MeshDrawPSInstance = SponzaMeshDrawMirrorSamplerPSInstance;
		}
		else
		{
			auto SponzaMeshDrawPSInstance = SponzaMeshDrawPS.MakeTemplatedMaterial();
			SponzaMeshDrawPSInstance->Parameter.DiffuseTexture = Model.Material->DiffuseTexture ? Model.Material->DiffuseTexture->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
			SponzaMeshDrawPSInstance->Parameter.NormalTexture = Model.Material->NormalsTexture ? Model.Material->NormalsTexture->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
			SponzaMeshDrawPSInstance->Parameter.EmissiveTexture = Model.Material->EmissionColorTexture ? Model.Material->EmissionColorTexture->GetTextureSRV() : DummyBlackTexture->GetTextureSRV();
			SponzaMeshDrawPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.Metalic
				= Model.Material->ConstantMetalicFactor;
			SponzaMeshDrawPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.Roughness
				= Model.Material->ConstantRoughnessFactor;
			MeshDrawPSInstance = SponzaMeshDrawPSInstance;
		}

		eastl::array<FD3D12Material*, EShaderFrequency::NumShaderFrequency> ShaderList{};
		ShaderList[EShaderFrequency::Vertex] = MeshDrawVSInstance;
		ShaderList[EShaderFrequency::Pixel] = MeshDrawPSInstance;
		FBoundShaderSet BoundShaderSet{ ShaderList };
		DrawDesc.BoundShaderSet = BoundShaderSet;

		for(uint32_t InstanceIndex = 0; InstanceIndex < Model.InstanceLocalToWorldMatrixList.size(); ++InstanceIndex)
		{
			const Matrix TranslationMatrix = Matrix::CreateTranslation(Model.CustomData.Transform.Position);
			const Matrix RotationMatrix = Matrix::CreateFromQuaternion(Model.CustomData.Transform.Rotation);
			const Matrix ScaleMatrix = Matrix::CreateScale(Model.CustomData.Transform.Scale);

			//
			RenderScene.AddPrimitive(
				true,
				Model.Mesh->AABB,
				EPrimitiveFlag::CacheMeshDrawCommand | EPrimitiveFlag::AllowMergeMeshDrawCommand,
				//Model.InstanceLocalToWorldMatrixList[InstanceIndex], @ TODO : Bistro asset has wrong matrix data, so we use this temporally
				ScaleMatrix * RotationMatrix * TranslationMatrix,
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
		float Speed = GTimeDelta * 0.5f;

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
			View.Transform.Translate(Vector3::Forward *Speed * 50.0f, ESpace::Self);
		}
		else if (FD3D12Window::SKeyPressed)
		{
			View.Transform.Translate(Vector3::Backward *Speed * 50.0f, ESpace::Self);
		}

		if (FD3D12Window::AKeyPressed)
		{
			View.Transform.Translate(Vector3::Left *Speed * 50.0f, ESpace::Self);
		}
		else if (FD3D12Window::DKeyPressed)
		{
			View.Transform.Translate(Vector3::Right *Speed * 50.0f, ESpace::Self);
		}
		Matrix ViewProjMat = View.GetViewPerspectiveProjectionMatrix(90.0f, SwapChain->GetWidth(), SwapChain->GetHeight());
		Matrix ViewMat = View.Get3DViewMatrices();
		Matrix ProjMat = View.GetPerspectiveProjectionMatrix(90.0f, SwapChain->GetWidth(), SwapChain->GetHeight());
		ViewConstantBuffer.MemberVariables.ViewWorldPosition = Vector4{ View.Transform.Position.x, View.Transform.Position.y, View.Transform.Position.z, 0.0f };
		ViewConstantBuffer.MemberVariables.ViewMatrix = ViewMat;
		ViewConstantBuffer.MemberVariables.InvViewMatrix = ViewMat.Invert();
		ViewConstantBuffer.MemberVariables.ProjectionMatrix = ProjMat;
		ViewConstantBuffer.MemberVariables.InvProjectionMatrix = ProjMat.Invert();
		ViewConstantBuffer.MemberVariables.ViewProjectionMatrix = ViewProjMat;
		ViewConstantBuffer.MemberVariables.InvViewProjectionMatrix = ViewProjMat.Invert();
		ViewConstantBuffer.MemberVariables.PrevViewProjectionMatrix = ViewProjMat;
		ViewConstantBuffer.FlushShadowDataIfDirty();
	}
}

bool D3D12TestRenderer::Draw()
{
	FRenderer::Draw();

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();

	if (!bSetupEnvCubemap)
	{
		CD3DX12_VIEWPORT Viewport{ 0.0f, 0.0f, static_cast<float>(GCubemapSize), static_cast<float>(GCubemapSize) };
		CD3DX12_RECT Rect{ 0, 0, static_cast<LONG>(GCubemapSize), static_cast<LONG>(GCubemapSize) };

		CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetViewports(1, &Viewport);
		CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetScissorRects(1, &Rect);

		for (int32 CubeMapFaceIndex = 0; CubeMapFaceIndex < CUBEMAP_FACE_COUNT; ++CubeMapFaceIndex)
		{
			D3D12_RENDER_TARGET_VIEW_DESC EnvCubemapRTVDesc{};
			MEM_ZERO(EnvCubemapRTVDesc);
			EnvCubemapRTVDesc.Format = EnvCubemap->GetDesc().Format;
			EnvCubemapRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			EnvCubemapRTVDesc.Texture2DArray.MipSlice = 0;
			EnvCubemapRTVDesc.Texture2DArray.FirstArraySlice = CubeMapFaceIndex;
			EnvCubemapRTVDesc.Texture2DArray.ArraySize = 1;
			EnvCubemapRTVDesc.Texture2DArray.PlaneSlice = 0;

			// set each cube face to render target
			CurrentFrameCommandContext.StateCache.SetRenderTargets({ EnvCubemap->GetRTV(EnvCubemapRTVDesc) });
			CurrentFrameCommandContext.StateCache.SetDepthStencilTarget(nullptr);

			eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> VertexBufferViewList{};
			VertexBufferViewList.emplace_back(GScreenDrawPositionBuffer->GetVertexBufferView());
			VertexBufferViewList.emplace_back(GScreenDrawUVBuffer->GetVertexBufferView());

			CurrentFrameCommandContext.StateCache.SetVertexBufferViewList(VertexBufferViewList);
			CurrentFrameCommandContext.StateCache.SetIndexBufferView(GScreenDrawIndexBuffer->GetIndexBufferView());

			FD3D12PSOInitializer::FDrawDesc DrawDesc;

			D3D12_INPUT_LAYOUT_DESC InputDesc{ ScreenDrawInputElementDescs, _countof(ScreenDrawInputElementDescs) };
			DrawDesc.Desc.InputLayout = InputDesc;
			DrawDesc.Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

			FMeshDrawArgument MeshDrawArgument;
			MeshDrawArgument.IndexCountPerInstance = 6;
			MeshDrawArgument.InstanceCount = 1;
			MeshDrawArgument.StartIndexLocation = 0;
			MeshDrawArgument.BaseVertexLocation = 0;
			MeshDrawArgument.StartInstanceLocation = 0;

			auto RenderCubemapVSInstance = RenderCubemapVS.MakeTemplatedMaterial();
			auto RenderCubemapPSInstance = RenderCubemapPS.MakeTemplatedMaterial();

			RenderCubemapVSInstance->Parameter.GlobalConstantBuffer.MemberVariables.PosScaleUVScale
				= Vector4{ static_cast<float>(EnvCubemap->GetDesc().Width), static_cast<float>(EnvCubemap->GetDesc().Height), static_cast<float>(EnvCubemap->GetDesc().Width), static_cast<float>(EnvCubemap->GetDesc().Height) };

			RenderCubemapVSInstance->Parameter.GlobalConstantBuffer.MemberVariables.InvTargetSizeAndTextureSize
				= Vector4{ 1.0f / EnvCubemap->GetDesc().Width, 1.0f / EnvCubemap->GetDesc().Height, 1.0f / static_cast<float>(EnvCubemap->GetDesc().Width), 1.0f / static_cast<float>(EnvCubemap->GetDesc().Height) };
			RenderCubemapPSInstance->Parameter.HDREnvMapTexture = HDREnvMapTexture->GetTextureSRV();

			Vector3 CubemapTarget[CUBEMAP_FACE_COUNT]{
				Vector3::Forward,
				Vector3::Backward,
				Vector3::Right,
				Vector3::Left,
				Vector3::Up,
				Vector3::Down
			};

			Vector3 CubemapUp[CUBEMAP_FACE_COUNT]{
				Vector3::Up,
				Vector3::Up,
				Vector3::Up,
				Vector3::Up,
				Vector3::Backward,
				Vector3::Forward
			};

			RenderCubemapPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.CubemapSize = Vector2(GCubemapSize, GCubemapSize);
			RenderCubemapPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.CubemapFaceIndex = CubeMapFaceIndex;

			FD3D12SRVDesc SRVDesc{};
			SRVDesc.ShaderParameterResourceType = EShaderParameterResourceType::Texture;
			D3D12_SHADER_RESOURCE_VIEW_DESC Desc{};
			MEM_ZERO(Desc);
			Desc.Format = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			Desc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
			Desc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 0, 0, 0);
			Desc.Texture2D.MostDetailedMip = 0;
			Desc.Texture2D.MipLevels = -1;
			Desc.Texture2D.PlaneSlice = 0;
			Desc.Texture2D.ResourceMinLODClamp = 0.0f;
			SRVDesc.Desc = Desc;

			eastl::array<FD3D12Material*, EShaderFrequency::NumShaderFrequency> ShaderList{};
			ShaderList[EShaderFrequency::Vertex] = RenderCubemapVSInstance;
			ShaderList[EShaderFrequency::Pixel] = RenderCubemapPSInstance;
			FBoundShaderSet BoundShaderSet{ ShaderList };
			DrawDesc.BoundShaderSet = BoundShaderSet;
			CurrentFrameCommandContext.StateCache.SetPSODrawDesc(DrawDesc);

			FD3D12PSOInitializer::FPassDesc BasePassPSODesc{};
			BasePassPSODesc.Desc.SampleMask = UINT_MAX;
			BasePassPSODesc.Desc.NumRenderTargets = 1;
			BasePassPSODesc.Desc.RTVFormats[0] = EnvCubemap->GetDesc().Format;
			BasePassPSODesc.Desc.SampleDesc.Count = 1;
			BasePassPSODesc.Desc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
			BasePassPSODesc.Desc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
			BasePassPSODesc.Desc.RasterizerState.FrontCounterClockwise = true;
			BasePassPSODesc.Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
			BasePassPSODesc.Desc.DepthStencilState.DepthEnable = false;
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
		}

		CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(EnvCubemap->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		bSetupEnvCubemap = true;
	}


	CurrentFrameCommandContext.StateCache.SetRenderTargets(
		{ GBufferManager.GBufferA.get(), GBufferManager.GBufferB.get(), GBufferManager.GBufferC.get() }
	);
	CurrentFrameCommandContext.StateCache.SetDepthEnable(true);
	CurrentFrameCommandContext.StateCache.SetDepthStencilTarget(GBufferManager.Depth.get());
	GBufferManager.GBufferA->ClearRenderTargetView(CurrentFrameCommandContext);
	GBufferManager.GBufferB->ClearRenderTargetView(CurrentFrameCommandContext);
	GBufferManager.GBufferC->ClearRenderTargetView(CurrentFrameCommandContext);
	GBufferManager.Depth->ClearDepthStencilView(CurrentFrameCommandContext);

	CD3DX12_VIEWPORT Viewport{ 0.0f, 0.0f, static_cast<float>(SwapChain->GetWidth()), static_cast<float>(SwapChain->GetHeight()) };
	CD3DX12_RECT Rect{ 0, 0, static_cast<LONG>(SwapChain->GetWidth()), static_cast<LONG>(SwapChain->GetHeight()) };

	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetViewports(1, &Viewport);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->RSSetScissorRects(1, &Rect);

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

		auto ScreenDrawVSInstance = ScreenDrawVS.MakeTemplatedMaterial();
		auto DeferredShadingPSInstance = DeferredShadingPS.MakeTemplatedMaterial();

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
		Desc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 0, 0, 0);
		Desc.Texture2D.MostDetailedMip = 0;
		Desc.Texture2D.MipLevels = -1;
		Desc.Texture2D.PlaneSlice = 0;
		Desc.Texture2D.ResourceMinLODClamp = 0.0f;
		SRVDesc.Desc = Desc;
		DeferredShadingPSInstance->Parameter.DepthTexture = GBufferManager.Depth->GetSRV(SRVDesc);

		Vector3 DirectionalLightYawPitchRoll = GDirectionalLightYawPitchRoll;
		Vector3 LightDirection = Vector3::Transform(
			Vector3::Up,
			Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(DirectionalLightYawPitchRoll.x), XMConvertToRadians(DirectionalLightYawPitchRoll.y), XMConvertToRadians(DirectionalLightYawPitchRoll.z))
		);
		LightDirection.Normalize();

		DeferredShadingPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.LightDirection = LightDirection;
		DeferredShadingPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.LightColor = GDirectionLightColor;
		DeferredShadingPSInstance->Parameter.GlobalConstantBuffer.MemberVariables.PosScaleUVScale =
			Vector4{ static_cast<float>(SwapChain->GetWidth()), static_cast<float>(SwapChain->GetHeight()), static_cast<float>(SwapChain->GetWidth()), static_cast<float>(SwapChain->GetHeight()) };

		eastl::array<FD3D12Material*, EShaderFrequency::NumShaderFrequency> ShaderList{};
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
