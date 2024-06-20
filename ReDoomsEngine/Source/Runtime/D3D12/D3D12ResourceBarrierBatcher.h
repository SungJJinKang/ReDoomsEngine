#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12CommandList;

// why batch(group) resource barrier? : to minimize overhead. the worst case can be picked instaed of sequentially going through all barrier.
// 
// references : 
// https://developer.nvidia.com/dx12-dos-and-donts
// https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc2019/presentations/Pettineo_Matt_Breaking%20Down%20Barriers.pdf
// https://gpuopen.com/wp-content/uploads/2016/03/Practical_DX12_Programming_Model_and_Hardware_Capabilities.pdf
// https://gpuopen.com/wp-content/uploads/2016/03/Practical_DX12_Programming_Model_and_Hardware_Capabilities.pdf
//
// 
class FD3D12ResourceBarrierBatcher
{
public:

	void AddBarrier(const CD3DX12_RESOURCE_BARRIER& InBarrier);
	void AddBarrier(const eastl::vector<CD3DX12_RESOURCE_BARRIER>& InBarriers);
	void Flush(FD3D12CommandList& InCommandList);

private:

	eastl::vector<CD3DX12_RESOURCE_BARRIER> PendingResourceBarriers;

};

