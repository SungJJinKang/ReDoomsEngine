#include "GlobalResources.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"

eastl::shared_ptr<FD3D12VertexIndexBufferResource> GScreenDrawPositionBuffer{};
eastl::shared_ptr<FD3D12VertexIndexBufferResource> GScreenDrawUVBuffer{};
eastl::shared_ptr<FD3D12VertexIndexBufferResource> GScreenDrawIndexBuffer{};

void InitGlobalResources(FD3D12CommandContext& InCommandContext)
{
	// we should maintain datas until be uploaded to gpu
	eastl::vector<Vector2> ScreenDrawPositionData = {
		Vector2(-1.0f, -1.0f),
		Vector2(-1.0f, 1.0f),
		Vector2(1.0f, 1.0f),
		Vector2(1.0f, -1.0f)
	};

	eastl::vector<Vector2> ScreenDrawUVData = {
		Vector2(0.0f, 0.0f),
		Vector2(0.0f, 1.0f),
		Vector2(1.0f, 1.0f),
		Vector2(1.0f, 0.0f)
	};

	eastl::vector<uint32> ScreenDrawIndexData = {
		0, 1, 2,
		0, 2, 3
	};

	GScreenDrawPositionBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(
		InCommandContext, reinterpret_cast<const uint8* const>(ScreenDrawPositionData.data()), ScreenDrawPositionData.size() * sizeof(Vector2), sizeof(Vector2));
	GScreenDrawPositionBuffer->SetDebugNameToResource(EA_WCHAR("GScreenDrawPositionBuffer"));

	GScreenDrawUVBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(
		InCommandContext, reinterpret_cast<const uint8* const>(ScreenDrawUVData.data()), ScreenDrawUVData.size() * sizeof(Vector2), sizeof(Vector2));
	GScreenDrawUVBuffer->SetDebugNameToResource(EA_WCHAR("GScreenDrawUVBuffer"));

	GScreenDrawIndexBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticIndexBuffer(
		InCommandContext, reinterpret_cast<const uint8* const>(ScreenDrawIndexData.data()), ScreenDrawIndexData.size() * sizeof(uint32), sizeof(uint32));
	GScreenDrawIndexBuffer->SetDebugNameToResource(EA_WCHAR("GScreenDrawIndexBuffer"));
}
