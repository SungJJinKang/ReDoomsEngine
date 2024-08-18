#pragma once
#include "CommonInclude.h"
#include "D3D12Resource/D3D12Resource.h"

extern eastl::shared_ptr<FD3D12VertexIndexBufferResource> GScreenDrawPositionBuffer;
extern eastl::shared_ptr<FD3D12VertexIndexBufferResource> GScreenDrawUVBuffer;
extern eastl::shared_ptr<FD3D12VertexIndexBufferResource> GScreenDrawIndexBuffer;

void InitGlobalResources(FD3D12CommandContext& InCommandContext);