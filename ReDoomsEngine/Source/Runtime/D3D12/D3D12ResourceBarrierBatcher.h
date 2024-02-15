#pragma once

// why batch(group) resource barrier? : to minimize overhead. the worst case can be picked instaed of sequentially going through all barrier.
// 
// references : 
// https://developer.nvidia.com/dx12-dos-and-donts
// https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc2019/presentations/Pettineo_Matt_Breaking%20Down%20Barriers.pdf
// https://gpuopen.com/wp-content/uploads/2016/03/Practical_DX12_Programming_Model_and_Hardware_Capabilities.pdf
// https://gpuopen.com/wp-content/uploads/2016/03/Practical_DX12_Programming_Model_and_Hardware_Capabilities.pdf
//
// 
class D3D12ResourceBarrierBatcher
{
};

