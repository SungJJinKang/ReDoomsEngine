#include "D3D12StateCache.h"

#include "D3D12Device.h"
#include "D3D12CommandContext.h"
#include "D3D12Shader.h"
#include "D3D12Resource/D3D12Resource.h"

#include "D3D12RootSignature.h"
#include "D3D12Shader.h"
#include "D3D12Resource/D3D12Resource.h"
#include "D3D12CommandList.h"
#include "Renderer.h"
#include "EASTL/sort.h"

void FD3D12StateCache::SetPSO(const FD3D12PSOInitializer& InPSOInitializer)
{
	EA_ASSERT(InPSOInitializer.CachedHash != 0);
	if (CachedPSOInitializer.CachedHash != InPSOInitializer.CachedHash)
	{
		CachedPSOInitializer = InPSOInitializer;
		SetRootSignature(CachedPSOInitializer.BoundShaderSet.GetRootSignature());
		bIsPSODirty = true;
	}
}

void FD3D12StateCache::SetRootSignature(FD3D12RootSignature* const InRootSignature)
{
	if (CachedRootSignature != InRootSignature)
	{
		CachedRootSignature = InRootSignature;
		bIsRootSignatureDirty = true;
	}
}

void FD3D12StateCache::SetSRVs(const EShaderFrequency InShaderFrequency, const eastl::array<FShaderParameterShaderResourceView*, MAX_SRVS>& BindPointInfos)
{
	// @todo : check if it is dirty
	CachedSRVBindPointInfosOfFrequencies[InShaderFrequency] = BindPointInfos;
	bIsSRVDirty = true;
}

void FD3D12StateCache::SetUAVs(const EShaderFrequency InShaderFrequency, const eastl::array<FShaderParameterShaderResourceView*, MAX_UAVS>& BindPointInfos)
{
	// @todo : check if it is dirty
	CachedUAVBindPointInfosOfFrequencies[InShaderFrequency] = BindPointInfos;
	bIsUAVDirty = true;
}

void FD3D12StateCache::SetConstantBuffer(const EShaderFrequency InShaderFrequency, const eastl::array<FShaderParameterConstantBuffer*, MAX_ROOT_CBV>& BindPointInfos)
{
	// @todo : check if it is dirty
	CachedConstantBufferBindPointInfosOfFrequencies[InShaderFrequency] = BindPointInfos;
	bIsRootCBVDirty = true;
}

void FD3D12StateCache::ApplyPSO(FD3D12CommandList& InCommandList)
{
	InCommandList.GetD3DCommandList()->SetPipelineState(FD3D12PSOManager::GetInstance()->GetOrCreatePSO(CachedPSOInitializer)->PSOObject.Get());

	bIsPSODirty = false;
}

void FD3D12StateCache::ApplyRootSignature(FD3D12CommandList& InCommandList)
{
	InCommandList.GetD3DCommandList()->SetGraphicsRootSignature(CachedPSOInitializer.BoundShaderSet.GetRootSignature()->RootSignature.Get());

	bIsRootSignatureDirty = false;
}

void FD3D12StateCache::ApplyDescriptorHeap(FD3D12CommandList& InCommandList)
{
	eastl::vector<ID3D12DescriptorHeap*> D3D12DescriptorHeaps;
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

		if (SlotsNeeded > 0)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor = BaseHeapBlcok.CPUDescriptorHandle().Offset(FirstSlotIndex);
			eastl::array<D3D12_CPU_DESCRIPTOR_HANDLE, MAX_SRVS> SrcDescriptors;

			for (uint32_t SlotIndex = 0; SlotIndex < SlotsNeeded; ++SlotIndex)
			{
				FShaderParameterShaderResourceView* ShaderParameterShaderResourceView = CachedSRVBindPointInfosOfFrequencies[FrequencyIndex][SlotIndex];
				if (ShaderParameterShaderResourceView && ShaderParameterShaderResourceView->GetTargetSRV())
				{
					EA_ASSERT(SlotIndex == ShaderParameterShaderResourceView->GetReflectionData().BindPoint);
					FD3D12ShaderResourceView* const SRV = dynamic_cast<FD3D12ShaderResourceView*>(ShaderParameterShaderResourceView->GetTargetSRV());
					SrcDescriptors[SlotIndex] = SRV->GetDescriptorHeapBlock().CPUDescriptorHandle();
				}
				else
				{
					SrcDescriptors[SlotIndex] = FD3D12ShaderResourceView::NullSRV()->GetDescriptorHeapBlock().CPUDescriptorHandle();
				}
			}

			GetD3D12Device()->CopyDescriptors(1, &DestDescriptor, &SlotsNeeded, SlotsNeeded, SrcDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			const D3D12_GPU_DESCRIPTOR_HANDLE BindDescriptor = BaseHeapBlcok.GPUDescriptorHandle().Offset(FirstSlotIndex);
			InCommandList.GetD3DCommandList()->SetGraphicsRootDescriptorTable(CachedRootSignature->SRVBindSlot[FrequencyIndex], BindDescriptor);

			OutUsedBlockCount += SlotsNeeded;
		}
 	}
	bIsSRVDirty = false;
}

void FD3D12StateCache::ApplyUAVs(FD3D12CommandList& InCommandList, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount)
{


	bIsUAVDirty = false;
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
				if (ShaderParameterConstantBuffer)
				{
					FD3D12ConstantBufferResource* const ConstantBufferResource = ShaderParameterConstantBuffer->GetConstantBufferResource();
					EA_ASSERT(ConstantBufferResource);
					ConstantBufferResource->Versioning();
					ShaderParameterConstantBuffer->FlushShadowData(ConstantBufferResource->GetMappedAddress());

					const D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = ConstantBufferResource->GPUVirtualAddress();
					EA_ASSERT(IsAligned(GPUVirtualAddress, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
					EA_ASSERT(ShaderParameterConstantBuffer->GetReflectionData()->ResourceBindingDesc.BindPoint == CBVRegisterIndex);
					InCommandList.GetD3DCommandList()->SetGraphicsRootConstantBufferView(BaseIndex + CBVRegisterIndex, GPUVirtualAddress);

					RootConstantBufferViewCount += 1;
				}
			}
		}
	}

	EA_ASSERT(RootConstantBufferViewCount <= MAX_ROOT_CBV);
	bIsRootCBVDirty = false;
}


void FD3D12StateCache::Flush(FD3D12CommandList& InCommandList)
{
	FD3D12DescriptorHeapBlock BaseHeapBlcok;

	if (bIsSRVDirty || bIsUAVDirty)
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

		BaseHeapBlcok = FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOnlineDescriptorHeapContainer.ReserveDescriptorHeapBlock(ReservedDescriptorCount);
	}

	uint32_t OutUsedBlockCount = 0;

	if (bIsPSODirty)
	{
		ApplyPSO(InCommandList);
	}
	if (bIsRootSignatureDirty)
	{
		ApplyRootSignature(InCommandList);
	}
	if (bNeedToSetDescriptorHeaps)
	{
		ApplyDescriptorHeap(InCommandList);
	}
	if (bIsSRVDirty)
	{
		ApplySRVs(InCommandList, BaseHeapBlcok, OutUsedBlockCount);
	}
	if (bIsUAVDirty)
	{
		ApplyUAVs(InCommandList, BaseHeapBlcok, OutUsedBlockCount);
	}
	if (bIsRootCBVDirty)
	{
		ApplyConstantBuffers(InCommandList);
	}
}

void FD3D12StateCache::ResetForNewCommandlist()
{
	CachedPSOInitializer.Reset();
	CachedRootSignature = nullptr;

	bIsPSODirty = true;
	bIsRootSignatureDirty = true;
	bNeedToSetDescriptorHeaps = true;
	bIsSRVDirty = true;
	bIsUAVDirty = true;
// 	bIsRTVDirty = true;
// 	bIsDSVDirty = true;
	bIsRootCBVDirty = true;

	for (auto& CachedSRVBindPointInfos : CachedSRVBindPointInfosOfFrequencies)
	{
		MEM_ZERO(CachedSRVBindPointInfos);
	}

	for (auto& CachedUAVBindPointInfos : CachedUAVBindPointInfosOfFrequencies)
	{
		MEM_ZERO(CachedUAVBindPointInfos);
	}

// 	for (auto& CachedRTVBindPointInfos : CachedRTVBindPointInfosOfFrequencies)
// 	{
// 		CachedRTVBindPointInfos.clear();
// 	}
// 
// 	for (auto& CachedDSVBindPointInfos : CachedDSVBindPointInfosOfFrequencies)
// 	{
// 		CachedDSVBindPointInfos.clear();
// 	}

	for (auto& CachedConstantBufferBindPointInfos : CachedConstantBufferBindPointInfosOfFrequencies)
	{
		MEM_ZERO(CachedConstantBufferBindPointInfos);
	}
}
