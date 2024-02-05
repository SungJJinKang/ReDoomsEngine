#include "D3D12StateCache.h"

#include "D3D12CommandContext.h"
#include "D3D12Shader.h"
#include "D3D12Resource/D3D12Resource.h"

#include "D3D12RootSignature.h"
#include "D3D12Shader.h"
#include "D3D12Resource/D3D12Resource.h"
#include "D3D12CommandList.h"

void FD3D12StateCache::Reset()
{
}

void FD3D12StateCache::ApplyConstantBuffer(FD3D12CommandContext& const InCommandContext, const EShaderFrequency InShaderFrequency, const FD3D12RootSignature* const InRootSignature, const eastl::vector<FConstantBufferBindPointInfo>& BindPoints)
{
	// @todo : set only dirty slot

	const uint32_t BaseIndex = InRootSignature->RootCBVBindSlot[InShaderFrequency];

	for (const FConstantBufferBindPointInfo& BindInfo : BindPoints)
	{
		BindInfo.ConstantBufferResource->Versioning();
		BindInfo.ConstantBufferResource->FlushShadowData();

		const D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = BindInfo.ConstantBufferResource->GPUVirtualAddress();
		EA_ASSERT(IsAligned(GPUVirtualAddress, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		InCommandContext.GraphicsCommandList->GetD3DCommandList()->SetGraphicsRootConstantBufferView(BaseIndex + BindInfo.ReflectionData->ResourceBindingDesc.BindPoint, GPUVirtualAddress);
	}

}
