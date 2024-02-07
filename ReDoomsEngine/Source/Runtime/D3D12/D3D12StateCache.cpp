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

void FD3D12StateCache::SetTargetRootSignature(const FD3D12RootSignature* const InRootSignature)
{
	if (!TargetRootSignature)
	{
		TargetRootSignature = InRootSignature;
	}
	else
	{
		EA_ASSERT(TargetRootSignature == InRootSignature);
	}
}

void FD3D12StateCache::SetSRVs(FD3D12CommandContext& const InCommandContext, const EShaderFrequency InShaderFrequency, const FD3D12RootSignature* const InRootSignature, const eastl::vector<FViewBindPointInfo>& BindPointInfos)
{
	SetTargetRootSignature(InRootSignature);

	CachedSRVBindPointInfosOfFrequencies[InShaderFrequency].insert(CachedSRVBindPointInfosOfFrequencies[InShaderFrequency].end(), BindPointInfos.begin(), BindPointInfos.end());
}

void FD3D12StateCache::SetUAVs(FD3D12CommandContext& const InCommandContext, const EShaderFrequency InShaderFrequency, const FD3D12RootSignature* const InRootSignature, const eastl::vector<FViewBindPointInfo>& BindPointInfos)
{
	SetTargetRootSignature(InRootSignature);

	CachedUAVBindPointInfosOfFrequencies[InShaderFrequency].insert(CachedUAVBindPointInfosOfFrequencies[InShaderFrequency].end(), BindPointInfos.begin(), BindPointInfos.end());
}

void FD3D12StateCache::SetConstantBuffer(FD3D12CommandContext& const InCommandContext, const EShaderFrequency InShaderFrequency, const FD3D12RootSignature* const InRootSignature, const eastl::vector<FConstantBufferBindPointInfo>& BindPointInfos)
{
	SetTargetRootSignature(InRootSignature);

	CachedConstantBufferBindPointInfosOfFrequencies[InShaderFrequency].insert(CachedConstantBufferBindPointInfosOfFrequencies[InShaderFrequency].end(), BindPointInfos.begin(), BindPointInfos.end());

}

void FD3D12StateCache::ApplySRVs(FD3D12CommandContext& const InCommandContext, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount)
{
 	for (uint8_t FrequencyIndex = 0; FrequencyIndex < EShaderFrequency::NumShaderFrequency; ++FrequencyIndex)
 	{
		uint32 FirstSlotIndex = OutUsedBlockCount;
		const uint32_t SlotsNeeded = TargetRootSignature->Stage[FrequencyIndex].MaxSRVCount;

		if (SlotsNeeded > 0)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor = BaseHeapBlcok.CPUDescriptorHandle().Offset(FirstSlotIndex);
			eastl::array<D3D12_CPU_DESCRIPTOR_HANDLE, MAX_SRVS> SrcDescriptors;

			// @todo clean with default srv
			// SrcDescriptors.fill(nullsrv);

			for (FViewBindPointInfo& BindInfo : CachedSRVBindPointInfosOfFrequencies[FrequencyIndex])
			{
				if (BindInfo.ResourceView)
				{
					FD3D12ShaderResourceView* SRV = dynamic_cast<FD3D12ShaderResourceView*>(BindInfo.ResourceView);
					EA_ASSERT(BindInfo.InputBindDesc.BindCount == 1);
					SrcDescriptors[BindInfo.InputBindDesc.BindPoint] = SRV->GetDescriptorHeapBlock().CPUDescriptorHandle();
				}
			}

			GetD3D12Device()->CopyDescriptors(1, &DestDescriptor, &SlotsNeeded, SlotsNeeded, SrcDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			const D3D12_GPU_DESCRIPTOR_HANDLE BindDescriptor = BaseHeapBlcok.GPUDescriptorHandle().Offset(FirstSlotIndex);
			InCommandContext.GraphicsCommandList->GetD3DCommandList()->SetGraphicsRootDescriptorTable(TargetRootSignature->SRVBindSlot[FrequencyIndex], BindDescriptor);

			OutUsedBlockCount += SlotsNeeded;
		}
 	}
}

void FD3D12StateCache::ApplyUAVs(FD3D12CommandContext& const InCommandContext, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount)
{
}

void FD3D12StateCache::ApplyConstantBuffers(FD3D12CommandContext& const InCommandContext)
{
	uint32_t RootConstantBufferViewCount = 0;

	for (uint8_t FrequencyIndex = 0; FrequencyIndex < EShaderFrequency::NumShaderFrequency; ++FrequencyIndex)
	{
		const uint32_t BaseIndex = TargetRootSignature->RootCBVBindSlot[FrequencyIndex];

		for (const FConstantBufferBindPointInfo& BindInfo : CachedConstantBufferBindPointInfosOfFrequencies[FrequencyIndex])
		{
			BindInfo.ConstantBufferResource->Versioning();
			BindInfo.ConstantBufferResource->FlushShadowData();

			const D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = BindInfo.ConstantBufferResource->GPUVirtualAddress();
			EA_ASSERT(IsAligned(GPUVirtualAddress, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
			EA_ASSERT(BindInfo.ReflectionData->ResourceBindingDesc.BindCount == 1); // @todo check case with mutilple bind count 
			InCommandContext.GraphicsCommandList->GetD3DCommandList()->SetGraphicsRootConstantBufferView(BaseIndex + BindInfo.ReflectionData->ResourceBindingDesc.BindPoint, GPUVirtualAddress);

			RootConstantBufferViewCount += 1;
		}
	}

	EA_ASSERT(RootConstantBufferViewCount <= MAX_ROOT_CBV);
}


void FD3D12StateCache::Flush(FD3D12CommandContext& const InCommandContext)
{
	uint32_t RequiredSRVSlotCount = 0;
	uint32_t RequiredURVSlotCount = 0;
	for (const eastl::vector<FViewBindPointInfo>& CachedSRVBindPointInfos : CachedSRVBindPointInfosOfFrequencies)
	{
		for (const FViewBindPointInfo& ViewBindPointInfo : CachedSRVBindPointInfos)
		{
			EA_ASSERT(ViewBindPointInfo.InputBindDesc.BindCount == 1); // @todo check case with mutilple bind count 
			RequiredSRVSlotCount += 1;
		}
	}

	for (const eastl::vector<FViewBindPointInfo>& CachedUAVBindPointInfos : CachedUAVBindPointInfosOfFrequencies)
	{
		for (const FViewBindPointInfo& ViewBindPointInfo : CachedUAVBindPointInfos)
		{
			EA_ASSERT(ViewBindPointInfo.InputBindDesc.BindCount == 1);
			RequiredURVSlotCount += 1;
		}
	}

	EA_ASSERT(RequiredSRVSlotCount <= MAX_SRVS);
	EA_ASSERT(RequiredURVSlotCount <= MAX_UAVS);

	uint32_t ReservedDescriptorCount = RequiredSRVSlotCount + RequiredURVSlotCount;

	const FD3D12DescriptorHeapBlock BaseHeapBlcok = InCommandContext.FrameResourceCounter->SrvUavOnlineDescriptorHeapContainer->ReserveDescriptorHeapBlock(ReservedDescriptorCount);
	uint32_t OutUsedBlockCount = 0;

	ApplySRVs(InCommandContext, BaseHeapBlcok, OutUsedBlockCount);
	ApplyUAVs(InCommandContext, BaseHeapBlcok, OutUsedBlockCount);
	ApplyConstantBuffers(InCommandContext);

	Reset();
}

void FD3D12StateCache::Reset()
{
	TargetRootSignature = nullptr;
	CurrentCbvSrvUavSlotIndex = 0;

	for (auto& CachedSRVBindPointInfos : CachedSRVBindPointInfosOfFrequencies)
	{
		CachedSRVBindPointInfos.clear();
	}

	for (auto& CachedUAVBindPointInfos : CachedUAVBindPointInfosOfFrequencies)
	{
		CachedUAVBindPointInfos.clear();
	}

	for (auto& CachedRTVBindPointInfos : CachedRTVBindPointInfosOfFrequencies)
	{
		CachedRTVBindPointInfos.clear();
	}

	for (auto& CachedDSVBindPointInfos : CachedDSVBindPointInfosOfFrequencies)
	{
		CachedDSVBindPointInfos.clear();
	}

	for (auto& CachedConstantBufferBindPointInfos : CachedConstantBufferBindPointInfosOfFrequencies)
	{
		CachedConstantBufferBindPointInfos.clear();
	}
}

void FD3D12StateCache::SetDescriptorHeaps(FD3D12CommandContext& const InCommandContext, eastl::vector<eastl::shared_ptr<FD3D12DescriptorHeap>> InDescriptorHeaps)
{
	EA_ASSERT(InDescriptorHeaps.size() > 0);

	bool bNeedSetDescriptorHeap = false;
	if (CachedSetDescriptorHeaps.size() != InDescriptorHeaps.size())
	{
		bNeedSetDescriptorHeap = true;
	}
	else
	{
		// CachedSetDescriptorHeaps is already sorted
		// eastl::sort(CachedSetDescriptorHeaps.begin(), CachedSetDescriptorHeaps.end()); 

		eastl::sort(InDescriptorHeaps.begin(), InDescriptorHeaps.end());

		for (size_t TestedHeapIndex = 0; TestedHeapIndex < CachedSetDescriptorHeaps.size(); ++TestedHeapIndex)
		{
			if (CachedSetDescriptorHeaps[TestedHeapIndex] != InDescriptorHeaps[TestedHeapIndex])
			{
				bNeedSetDescriptorHeap = true;
				break;
			}
		}
	}

	if (bNeedSetDescriptorHeap)
	{
		eastl::vector<ID3D12DescriptorHeap*> D3D12DescriptorHeaps;
		for (eastl::shared_ptr<FD3D12DescriptorHeap>& DescriptorHeap : InDescriptorHeaps)
		{
			D3D12DescriptorHeaps.emplace_back(DescriptorHeap->D3DDescriptorHeap.Get());
		}
		InCommandContext.GraphicsCommandList->GetD3DCommandList()->SetDescriptorHeaps(D3D12DescriptorHeaps.size(), D3D12DescriptorHeaps.data());

		CachedSetDescriptorHeaps = InDescriptorHeaps;
	}
}
