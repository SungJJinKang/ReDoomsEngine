#include "D3D12StateCache.h"

#include "D3D12Device.h"
#include "D3D12CommandContext.h"
#include "D3D12Shader.h"
#include "D3D12Resource/D3D12Resource.h"

#include "D3D12RootSignature.h"
#include "D3D12Shader.h"
#include "D3D12Resource/D3D12Resource.h"
#include "D3D12CommandList.h"
#include "Renderer/Renderer.h"
#include "EASTL/sort.h"

static TConsoleVariable<bool> GAlwaysInvalidateD3D12StateCache{ "r.AlwaysInvalidateD3D12StateCache", false };

void FD3D12StateCache::SetPSOInputLayout(const D3D12_INPUT_LAYOUT_DESC& InputLayoutDesc)
{
	bool bIsSame = false;

	if (CachedPSOInitializer.DrawDesc.Desc.InputLayout.NumElements == InputLayoutDesc.NumElements)
	{
		if (EA::StdC::Memcmp(CachedPSOInitializer.DrawDesc.Desc.InputLayout.pInputElementDescs, InputLayoutDesc.pInputElementDescs, sizeof(D3D12_INPUT_ELEMENT_DESC) * InputLayoutDesc.NumElements) == 0)
		{
			bIsSame = true;
		}
	}
	
	if (!bIsSame)
	{
		CachedPSOInitializer.DrawDesc.Desc.InputLayout = InputLayoutDesc;
;		bIsPSODirty = true;
	}
}

void FD3D12StateCache::SetRasterizeDesc(const CD3DX12_RASTERIZER_DESC& RasterizeDesc)
{
	bool bIsSame = false;

	if (EA::StdC::Memcmp(&(CachedPSOInitializer.PassDesc.Desc.RasterizerState), &RasterizeDesc, sizeof(CD3DX12_RASTERIZER_DESC)) == 0)
	{
		bIsSame = true;
	}

	if (!bIsSame)
	{
		CachedPSOInitializer.PassDesc.Desc.RasterizerState = RasterizeDesc;
		bIsPSODirty = true;
	}
}

void FD3D12StateCache::SetBlendDesc(const CD3DX12_BLEND_DESC& BlendDesc)
{
	bool bIsSame = false;

	if (EA::StdC::Memcmp(&(CachedPSOInitializer.PassDesc.Desc.BlendState), &BlendDesc, sizeof(CD3DX12_BLEND_DESC)) == 0)
	{
		bIsSame = true;
	}

	if (!bIsSame)
	{
		CachedPSOInitializer.PassDesc.Desc.BlendState = BlendDesc;
		bIsPSODirty = true;
	}
}

void FD3D12StateCache::SetDepthEnable(const bool bInEnable)
{
	bool bIsSame = false;

	if (CachedPSOInitializer.PassDesc.Desc.DepthStencilState.DepthEnable == bInEnable)
	{
		bIsSame = true;
	}

	if (!bIsSame)
	{
		CachedPSOInitializer.PassDesc.Desc.DepthStencilState.DepthEnable = bInEnable;
		bIsPSODirty = true;
	}
}

void FD3D12StateCache::SetStencilEnable(const bool bInEnable)
{
	bool bIsSame = false;

	if (CachedPSOInitializer.PassDesc.Desc.DepthStencilState.StencilEnable == bInEnable)
	{
		bIsSame = true;
	}

	if (!bIsSame)
	{
		CachedPSOInitializer.PassDesc.Desc.DepthStencilState.StencilEnable = bInEnable;
		bIsPSODirty = true;
	}
}

void FD3D12StateCache::SetBoundShaderSet(const FBoundShaderSet& InBoundShaderSet)
{
	EA_ASSERT(InBoundShaderSet.GetCachedHash().IsValid());
	if (CachedPSOInitializer.DrawDesc.BoundShaderSet.GetCachedHash() != InBoundShaderSet.GetCachedHash())
	{
		CachedPSOInitializer.DrawDesc.BoundShaderSet = InBoundShaderSet;
		bIsPSODirty = true;
	}
	else
	{
		CachedPSOInitializer.DrawDesc.BoundShaderSet = InBoundShaderSet; // Different shader instances can have same hash when its ShaderTemplate is same
	}
	SetRootSignature(CachedPSOInitializer.DrawDesc.BoundShaderSet.GetRootSignature());
}

void FD3D12StateCache::SetPSO(const FD3D12PSOInitializer& InPSOInitializer)
{
	SCOPED_CPU_TIMER(FD3D12StateCache_SetPSO)

	EA_ASSERT(InPSOInitializer.GetCachedHash() != 0);
	if (!(CachedPSOInitializer.IsValidHash()) || (CachedPSOInitializer.GetCachedHash() != InPSOInitializer.GetCachedHash()))
	{
		if (CachedPSOInitializer.DrawDesc.Desc.PrimitiveTopologyType != InPSOInitializer.DrawDesc.Desc.PrimitiveTopologyType)
		{
			bIsPrimitiveTopologyDirty = true;
		}

		CachedPSOInitializer = InPSOInitializer;
		bIsPSODirty = true;
	}
	SetBoundShaderSet(InPSOInitializer.DrawDesc.BoundShaderSet);
}

void FD3D12StateCache::SetRenderTargets(const eastl::array<FD3D12Texture2DResource*, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>& InRenderTargets)
{
	CachedRTVCount = 0;
	for (uint32_t RenderTargetIndex = 0; RenderTargetIndex < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++RenderTargetIndex)
	{
		FD3D12View* ResourceView = InRenderTargets[RenderTargetIndex] ? InRenderTargets[RenderTargetIndex]->GetRTV() : nullptr;
		CD3DX12_CPU_DESCRIPTOR_HANDLE InputRTVCPUHandle = InRenderTargets[RenderTargetIndex] ? ResourceView->GetDescriptorHeapBlock().CPUDescriptorHandle() : CD3DX12_CPU_DESCRIPTOR_HANDLE{};
		if (CachedRTVCPUHandleList[RenderTargetIndex].CPUDescriptorHandle != InputRTVCPUHandle)
		{
			CachedRTVCPUHandleList[RenderTargetIndex].CPUDescriptorHandle = InputRTVCPUHandle;
			CachedRTVCPUHandleList[RenderTargetIndex].ResourceView = ResourceView;
			bNeedToSetRTVAndDSV = true;
		}

		DXGI_FORMAT RTVFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		if (InRenderTargets[RenderTargetIndex])
		{
			const eastl::optional<D3D12_RENDER_TARGET_VIEW_DESC> RTVDesc = InRenderTargets[RenderTargetIndex]->GetRTV()->GetDesc();
			if (RTVDesc.has_value())
			{
				RTVFormat = RTVDesc->Format;
			}
			else
			{
				RTVFormat = InRenderTargets[RenderTargetIndex]->GetDesc().Format;
			}
		}

		if (CachedPSOInitializer.PassDesc.Desc.RTVFormats[RenderTargetIndex] != RTVFormat)
		{
			CachedPSOInitializer.PassDesc.Desc.RTVFormats[RenderTargetIndex] = RTVFormat;
			CachedPSOInitializer.PassDesc.Desc.NumRenderTargets = RenderTargetIndex + 1;
			bIsPSODirty = true;
		}
		
		if (InputRTVCPUHandle.ptr)
		{
			CachedRTVCount = RenderTargetIndex + 1;
		}
	}
}

void FD3D12StateCache::SetDepthStencilTarget(FD3D12Texture2DResource* const InDepthStencilTarget)
{
	FD3D12View* ResourceView = InDepthStencilTarget ? InDepthStencilTarget->GetDSV() : nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE InputDSVCPUHandle = InDepthStencilTarget ? ResourceView->GetDescriptorHeapBlock().CPUDescriptorHandle() : CD3DX12_CPU_DESCRIPTOR_HANDLE{};
	if (CachedDSVCPUHandle.CPUDescriptorHandle != InputDSVCPUHandle)
	{
		CachedDSVCPUHandle.CPUDescriptorHandle = InputDSVCPUHandle;
		CachedDSVCPUHandle.ResourceView = ResourceView;
		bNeedToSetRTVAndDSV = true;
	}

	DXGI_FORMAT DSVFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	if (InDepthStencilTarget)
	{
		const eastl::optional<D3D12_DEPTH_STENCIL_VIEW_DESC> DSVDesc = InDepthStencilTarget->GetDSV()->GetDesc();
		if (DSVDesc.has_value())
		{
			DSVFormat = DSVDesc->Format;
		}
		else
		{
			DSVFormat = InDepthStencilTarget->GetDesc().Format;
		}
	}

	if (CachedPSOInitializer.PassDesc.Desc.DSVFormat != DSVFormat)
	{
		CachedPSOInitializer.PassDesc.Desc.DSVFormat = DSVFormat;
		bIsPSODirty = true;
	}
}

void FD3D12StateCache::SetRootSignature(FD3D12RootSignature* const InRootSignature)
{
	if (CachedRootSignature != InRootSignature)
	{
		CachedRootSignature = InRootSignature;
		bIsRootSignatureDirty = true;

		DirtyFlagsOfSRVs.set();
		DirtyFlagsOfUAVs.set();
		bIsRootCBVDirty = true;
	}
}

void FD3D12StateCache::SetSRVs(const EShaderFrequency InShaderFrequency, const eastl::array<FD3D12ShaderResourceView*, MAX_SRVS>& BindPointInfos)
{
	bool bEqual = true;
	for (uint32_t SRVIndex = 0; SRVIndex < MAX_SRVS; ++SRVIndex)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE Handle = BindPointInfos[SRVIndex] ? BindPointInfos[SRVIndex]->GetDescriptorHeapBlock().CPUDescriptorHandle() : CD3DX12_CPU_DESCRIPTOR_HANDLE{};
		if (CachedSRVs[InShaderFrequency][SRVIndex].CPUDescriptorHandle != Handle)
		{
			bEqual = false;
			break;
		}
	}
	if (!bEqual)
	{
		DirtyFlagsOfSRVs[InShaderFrequency] = true;

		for (uint32_t SRVIndex = 0; SRVIndex < MAX_SRVS; ++SRVIndex)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE Handle = BindPointInfos[SRVIndex] ? BindPointInfos[SRVIndex]->GetDescriptorHeapBlock().CPUDescriptorHandle() : CD3DX12_CPU_DESCRIPTOR_HANDLE{};
			CachedSRVs[InShaderFrequency][SRVIndex].CPUDescriptorHandle = Handle;
			CachedSRVs[InShaderFrequency][SRVIndex].ResourceView = BindPointInfos[SRVIndex];
		}
	}
}

void FD3D12StateCache::SetUAVs(const EShaderFrequency InShaderFrequency, const eastl::array<FD3D12ShaderResourceView*, MAX_UAVS>& BindPointInfos)
{
	bool bEqual = true;
	for (uint32_t UAVIndex = 0; UAVIndex < MAX_UAVS; ++UAVIndex)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE Handle = BindPointInfos[UAVIndex] ? BindPointInfos[UAVIndex]->GetDescriptorHeapBlock().CPUDescriptorHandle() : CD3DX12_CPU_DESCRIPTOR_HANDLE{};
		if (CachedUAVs[InShaderFrequency][UAVIndex].CPUDescriptorHandle != Handle)
		{
			bEqual = false;
			break;
		}
	}
	if (!bEqual)
	{
		DirtyFlagsOfUAVs[InShaderFrequency] = true;

		for (uint32_t SRVIndex = 0; SRVIndex < MAX_UAVS; ++SRVIndex)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE Handle = BindPointInfos[SRVIndex] ? BindPointInfos[SRVIndex]->GetDescriptorHeapBlock().CPUDescriptorHandle() : CD3DX12_CPU_DESCRIPTOR_HANDLE{};
			CachedUAVs[InShaderFrequency][SRVIndex].CPUDescriptorHandle = Handle;
			CachedUAVs[InShaderFrequency][SRVIndex].ResourceView = BindPointInfos[SRVIndex];
		}
	}
}

void FD3D12StateCache::SetConstantBuffer(const EShaderFrequency InShaderFrequency, const eastl::array<FShaderParameterConstantBuffer*, MAX_ROOT_CBV>& BindPointInfos)
{
	CachedConstantBufferBindPointInfosOfFrequencies[InShaderFrequency] = BindPointInfos;
	bIsRootCBVDirty = true;
}

void FD3D12StateCache::SetVertexBufferViewList(const eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>& InVertexBufferViewList)
{
	SCOPED_CPU_TIMER(FD3D12StateCache_SetVertexBufferViewList)

	bool bNeedToSet = false;

	if (CachedVertexBufferViewList.size() == InVertexBufferViewList.size())
	{
		for (uint32_t VertexBufferViewIndex = 0; VertexBufferViewIndex < InVertexBufferViewList.size(); ++VertexBufferViewIndex)
		{
			if (
				(CachedVertexBufferViewList[VertexBufferViewIndex].BufferLocation != InVertexBufferViewList[VertexBufferViewIndex].BufferLocation) ||
				(CachedVertexBufferViewList[VertexBufferViewIndex].SizeInBytes != InVertexBufferViewList[VertexBufferViewIndex].SizeInBytes) ||
				(CachedVertexBufferViewList[VertexBufferViewIndex].StrideInBytes != InVertexBufferViewList[VertexBufferViewIndex].StrideInBytes)
			)
			{
				bNeedToSet = true;
				break;
			}
		}
	}
	else
	{
		bNeedToSet = true;
	}

	if (bNeedToSet)
	{
		CachedVertexBufferViewList = InVertexBufferViewList;
		bNeedToSetVertexBufferView = true;
	}
}

void FD3D12StateCache::SetIndexBufferView(const D3D12_INDEX_BUFFER_VIEW InIndexBufferView)
{
	SCOPED_CPU_TIMER(FD3D12StateCache_SetIndexBufferView)

	if (
		(CachedIndexBufferView.BufferLocation != InIndexBufferView.BufferLocation) ||
		(CachedIndexBufferView.SizeInBytes != InIndexBufferView.SizeInBytes) ||
		(CachedIndexBufferView.Format != InIndexBufferView.Format)
		)
	{
		CachedIndexBufferView = InIndexBufferView;
		bNeedToSetIndexBufferView = true;
	}
}

void FD3D12StateCache::ApplyPSO(FD3D12CommandList& InCommandList)
{
	CachedPSOInitializer.CacheHash();
	InCommandList.GetD3DCommandList()->SetPipelineState(FD3D12PSOManager::GetInstance()->GetOrCreatePSO(CachedPSOInitializer)->PSOObject.Get());

	bIsPSODirty = false;
}

static D3D12_PRIMITIVE_TOPOLOGY GetD3D12PrimitiveTopology(const D3D12_PRIMITIVE_TOPOLOGY_TYPE InPrimitiveTopologyType)
{
	D3D12_PRIMITIVE_TOPOLOGY PrimTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	switch (InPrimitiveTopologyType)
	{
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:
		{
			PrimTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		}
		default:
		{
			EA_ASSERT(false);
			break;
		}
	}

	return PrimTopology;
}

void FD3D12StateCache::ApplyPrimitiveTopologyDirty(FD3D12CommandList& InCommandList)
{
	InCommandList.GetD3DCommandList()->IASetPrimitiveTopology(GetD3D12PrimitiveTopology(CachedPSOInitializer.DrawDesc.Desc.PrimitiveTopologyType));

	bIsPrimitiveTopologyDirty = false;
}

void FD3D12StateCache::ApplyRTVAndDSV(FD3D12CommandList& InCommandList)
{
	EA_ASSERT(CachedRTVCount != UINT32_MAX);
	eastl::array<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> LocalCachedRTVCPUHandleList{};
	for (int32 Index = 0; Index < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++Index)
	{
		LocalCachedRTVCPUHandleList[Index] = CachedRTVCPUHandleList[Index].CPUDescriptorHandle;
	}

	InCommandList.GetD3DCommandList()->OMSetRenderTargets(CachedRTVCount, LocalCachedRTVCPUHandleList.data(), false, CachedDSVCPUHandle.CPUDescriptorHandle.ptr ? &(CachedDSVCPUHandle.CPUDescriptorHandle) : nullptr);

	bNeedToSetRTVAndDSV = false;
}

void FD3D12StateCache::ApplyRootSignature(FD3D12CommandList& InCommandList)
{
	InCommandList.GetD3DCommandList()->SetGraphicsRootSignature(CachedPSOInitializer.DrawDesc.BoundShaderSet.GetRootSignature()->RootSignature.Get());

	bIsRootSignatureDirty = false;
}

void FD3D12StateCache::ApplyDescriptorHeap(FD3D12CommandList& InCommandList)
{
	eastl::fixed_vector<ID3D12DescriptorHeap*, 1> D3D12DescriptorHeaps;
	D3D12DescriptorHeaps.emplace_back(FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOnlineDescriptorHeapContainer.GetOnlineHeap()->D3DDescriptorHeap.Get());
	InCommandList.GetD3DCommandList()->SetDescriptorHeaps(D3D12DescriptorHeaps.size(), D3D12DescriptorHeaps.data());

	bNeedToSetDescriptorHeaps = false;
}

void FD3D12StateCache::ApplySRVs(FD3D12CommandList& InCommandList, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount)
{
 	for (uint8_t FrequencyIndex = 0; FrequencyIndex < EShaderFrequency::NumShaderFrequency; ++FrequencyIndex)
 	{
		uint32 FirstSlotIndex = OutUsedBlockCount;
		const UINT SlotsNeeded = CachedRootSignature->Stage[FrequencyIndex].MaxSRVCount;

		if (SlotsNeeded > 0 && DirtyFlagsOfSRVs[FrequencyIndex])
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE DestDescriptor = BaseHeapBlcok.CPUDescriptorHandle().Offset(FirstSlotIndex, BaseHeapBlcok.DescriptorSize);
			eastl::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, MAX_SRVS> SrcDescriptors;

			for (uint32_t SlotIndex = 0; SlotIndex < SlotsNeeded; ++SlotIndex)
			{
				CD3DX12_CPU_DESCRIPTOR_HANDLE SRVCPUHandle = CachedSRVs[FrequencyIndex][SlotIndex].CPUDescriptorHandle;
				if (SRVCPUHandle.ptr != NULL)
				{
					SrcDescriptors[SlotIndex] = SRVCPUHandle;
				}
				else
				{
					SrcDescriptors[SlotIndex] = FD3D12ShaderResourceView::NullSRV()->GetDescriptorHeapBlock().CPUDescriptorHandle();
				}
			}

			if (SlotsNeeded == 1)
			{
				/*
					https://gpuopen.com/learn/rdna-performance-guide/#synchronization
					Prefer using CopyDescriptorsSimple / vkUpdateDescriptorSet over CopyDescriptors / vkUpdateDescriptorSetWithTemplate.
					This function has a better CPU cache hit rate.
				*/
				GetD3D12Device()->CopyDescriptorsSimple(1, DestDescriptor, SrcDescriptors[0], D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
			else
			{
				GetD3D12Device()->CopyDescriptors(1, &DestDescriptor, &SlotsNeeded, SlotsNeeded, SrcDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}

			const CD3DX12_GPU_DESCRIPTOR_HANDLE BindDescriptor = BaseHeapBlcok.GPUDescriptorHandle().Offset(FirstSlotIndex, BaseHeapBlcok.DescriptorSize);
			InCommandList.GetD3DCommandList()->SetGraphicsRootDescriptorTable(CachedRootSignature->SRVBindSlot[FrequencyIndex], BindDescriptor);

			OutUsedBlockCount += SlotsNeeded;
			DirtyFlagsOfSRVs[FrequencyIndex] = false;
		}
 	}
}

void FD3D12StateCache::ApplyUAVs(FD3D12CommandList& InCommandList, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount)
{

}

void FD3D12StateCache::ApplyConstantBuffers(FD3D12CommandList& InCommandList)
{
	uint32_t RootConstantBufferViewCount = 0;

	for (uint8_t FrequencyIndex = 0; FrequencyIndex < EShaderFrequency::NumShaderFrequency; ++FrequencyIndex)
	{
		const uint16_t CBVRegisterMask = CachedRootSignature->Stage[FrequencyIndex].CBVRegisterMask;
		const uint8_t BaseIndex = CachedRootSignature->RootCBVBindSlot[FrequencyIndex];

		for (uint16_t CBVRegisterIndex = 0; CBVRegisterIndex < MAX_ROOT_CBV; ++CBVRegisterIndex)
		{
			if (CBVRegisterMask & (1 << CBVRegisterIndex))
			{
				FShaderParameterConstantBuffer* ShaderParameterConstantBuffer = CachedConstantBufferBindPointInfosOfFrequencies[FrequencyIndex][CBVRegisterIndex];
				EA_ASSERT(!(ShaderParameterConstantBuffer->IsCulled()));
				if (ShaderParameterConstantBuffer)
				{
					FD3D12ConstantBufferResource* const ConstantBufferResource = ShaderParameterConstantBuffer->GetConstantBufferResource();
					EA_ASSERT(ConstantBufferResource);

					bool bNeedSetGraphicsRootConstantBufferView = false;
					// @todo doesn't need to copy shadow data if non-dynamic constant buffer
					EA_ASSERT_FORMATTED(!(ConstantBufferResource->IsDynamicBuffer()) ? !(ConstantBufferResource->IsShadowDataDirty()) : true,
						("ConstantBuffer(\"%s\")'s shadow data shouldn't be dirty at here, You should flush it directly before draw", ShaderParameterConstantBuffer->GetVariableName())
					);
					if (ConstantBufferResource->IsShadowDataDirty() && ConstantBufferResource->IsDynamicBuffer())
					{
						ConstantBufferResource->Versioning();
						ShaderParameterConstantBuffer->FlushShadowData();
						bNeedSetGraphicsRootConstantBufferView = true;
					}

					const D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = ConstantBufferResource->GPUVirtualAddress();
					if (CachedConstantBufferGPUVirtualAddressOfFrequencies[FrequencyIndex][CBVRegisterIndex] != GPUVirtualAddress)
					{
						bNeedSetGraphicsRootConstantBufferView = true;
					}

					if (bNeedSetGraphicsRootConstantBufferView)
					{
						EA_ASSERT(IsAligned(GPUVirtualAddress, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
						EA_ASSERT(ShaderParameterConstantBuffer->GetConstantBufferReflectionData()->ResourceBindingDesc.BindPoint == CBVRegisterIndex);
						InCommandList.GetD3DCommandList()->SetGraphicsRootConstantBufferView(BaseIndex + CBVRegisterIndex, GPUVirtualAddress);

						CachedConstantBufferGPUVirtualAddressOfFrequencies[FrequencyIndex][CBVRegisterIndex] = GPUVirtualAddress;
					}

					RootConstantBufferViewCount += 1;
				}
			}
		}
	}

	EA_ASSERT(RootConstantBufferViewCount <= MAX_ROOT_CBV);
	bIsRootCBVDirty = false;
}


void FD3D12StateCache::ApplyVertexBufferViewList(FD3D12CommandList& InCommandList)
{
	EA_ASSERT(CachedVertexBufferViewList.size() > 0);
	InCommandList.GetD3DCommandList()->IASetVertexBuffers(0, CachedVertexBufferViewList.size(), CachedVertexBufferViewList.data());
	bNeedToSetVertexBufferView = false;
}

void FD3D12StateCache::ApplyIndexBufferView(FD3D12CommandList& InCommandList)
{
	InCommandList.GetD3DCommandList()->IASetIndexBuffer(&CachedIndexBufferView);
	bNeedToSetIndexBufferView = false;
}

void FD3D12StateCache::Flush(FD3D12CommandContext& InCommandContext, const EPipeline InPipeline)
{
	SCOPED_CPU_TIMER(FD3D12StateCache_Flush)

	if ((bIsPSODirty || GAlwaysInvalidateD3D12StateCache) && CachedPSOInitializer.IsValid())
	{
		ApplyPSO(*(InCommandContext.GraphicsCommandList));
	}
	if (bIsPrimitiveTopologyDirty || GAlwaysInvalidateD3D12StateCache)
	{
		ApplyPrimitiveTopologyDirty(*(InCommandContext.GraphicsCommandList));
	}
	if (bNeedToSetDescriptorHeaps || GAlwaysInvalidateD3D12StateCache)
	{
		ApplyDescriptorHeap(*(InCommandContext.GraphicsCommandList));
	}
	if ((InPipeline == EPipeline::Graphics) && (bNeedToSetRTVAndDSV || GAlwaysInvalidateD3D12StateCache))
	{
		ApplyRTVAndDSV(*(InCommandContext.GraphicsCommandList));
	}
	if ((InPipeline == EPipeline::Graphics) && (bNeedToSetVertexBufferView || GAlwaysInvalidateD3D12StateCache))
	{
		ApplyVertexBufferViewList(*(InCommandContext.GraphicsCommandList));
	}
	if ((InPipeline == EPipeline::Graphics) && (bNeedToSetIndexBufferView || GAlwaysInvalidateD3D12StateCache) && CachedIndexBufferView.BufferLocation)
	{
		ApplyIndexBufferView(*(InCommandContext.GraphicsCommandList));
	}

	if (CachedRootSignature)
	{
		FD3D12DescriptorHeapBlock BaseHeapBlcok;

		if (DirtyFlagsOfSRVs.any() || DirtyFlagsOfUAVs.any() || GAlwaysInvalidateD3D12StateCache)
		{
			uint32_t RequiredSRVSlotCount = 0;
			uint32_t RequiredUAVSlotCount = 0;
			for (uint8_t FrequencyIndex = 0; FrequencyIndex < EShaderFrequency::NumShaderFrequency; ++FrequencyIndex)
			{
				RequiredSRVSlotCount += CachedRootSignature->Stage[FrequencyIndex].MaxSRVCount;
				RequiredUAVSlotCount += CachedRootSignature->Stage[FrequencyIndex].MaxUAVCount;
			}

			EA_ASSERT(RequiredSRVSlotCount <= MAX_SRVS);
			EA_ASSERT(RequiredUAVSlotCount <= MAX_UAVS);

			uint32_t ReservedDescriptorCount = RequiredSRVSlotCount + RequiredUAVSlotCount;

			BaseHeapBlcok = FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOnlineDescriptorHeapContainer.ReserveTransientDescriptorHeapBlock(ReservedDescriptorCount);
		}

		uint32_t OutUsedBlockCount = 0;

		for (uint32_t ShaderFrequencyIndex = 0; ShaderFrequencyIndex < EShaderFrequency::NumShaderFrequency; ++ShaderFrequencyIndex)
		{
			FD3D12ShaderInstance* ShaderInstance = CachedPSOInitializer.DrawDesc.BoundShaderSet.GetShaderInstanceList()[ShaderFrequencyIndex];
			if (ShaderInstance)
			{
				ShaderInstance->ApplyShaderParameter(InCommandContext);
			}
		}

		if (bIsRootSignatureDirty || GAlwaysInvalidateD3D12StateCache)
		{
			ApplyRootSignature(*(InCommandContext.GraphicsCommandList));
		}
		if (DirtyFlagsOfSRVs.any() || GAlwaysInvalidateD3D12StateCache)
		{
			ApplySRVs(*(InCommandContext.GraphicsCommandList), BaseHeapBlcok, OutUsedBlockCount);
		}
		if (DirtyFlagsOfUAVs.any() || GAlwaysInvalidateD3D12StateCache)
		{
			ApplyUAVs(*(InCommandContext.GraphicsCommandList), BaseHeapBlcok, OutUsedBlockCount);
		}
		if (bIsRootCBVDirty || GAlwaysInvalidateD3D12StateCache)
		{
			ApplyConstantBuffers(*(InCommandContext.GraphicsCommandList));
		}
	}
}

void FD3D12StateCache::ResetForNewCommandlist()
{
	MEM_ZERO(CachedPSOInitializer);
	CachedRootSignature = nullptr;

	bIsPSODirty = false;
	bIsPrimitiveTopologyDirty = false;
	bIsRootSignatureDirty = false;
	bIsRootCBVDirty = false;
	bNeedToSetDescriptorHeaps = true;
	bNeedToSetRTVAndDSV = true;
	bNeedToSetVertexBufferView = false;
	bNeedToSetIndexBufferView = false;

	DirtyFlagsOfSRVs.set();
	DirtyFlagsOfUAVs.set();
	MEM_ZERO(CachedSRVs);
	MEM_ZERO(CachedUAVs);
	MEM_ZERO(CachedConstantBufferGPUVirtualAddressOfFrequencies); // always set constant buffer view at first flush of new frame
	MEM_ZERO(CachedConstantBufferBindPointInfosOfFrequencies);
	MEM_ZERO(CachedVertexBufferViewList);
	MEM_ZERO(CachedIndexBufferView);
}

void FD3D12StateCache::ResetToDefault()
{
	CachedPSOInitializer.PassDesc.Desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	CachedPSOInitializer.PassDesc.Desc.RasterizerState.FrontCounterClockwise = true;
	CachedPSOInitializer.PassDesc.Desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	CachedPSOInitializer.PassDesc.Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	CachedPSOInitializer.PassDesc.Desc.DepthStencilState.DepthEnable = false;
	CachedPSOInitializer.PassDesc.Desc.DepthStencilState.StencilEnable = false;
	CachedPSOInitializer.PassDesc.Desc.SampleMask = UINT_MAX;
	CachedPSOInitializer.DrawDesc.Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	CachedPSOInitializer.PassDesc.Desc.NumRenderTargets = 1;
	CachedPSOInitializer.PassDesc.Desc.SampleDesc.Count = 1;
}
