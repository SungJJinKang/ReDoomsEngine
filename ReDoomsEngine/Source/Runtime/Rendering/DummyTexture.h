#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Resource/D3D12Resource.h"

extern eastl::shared_ptr<FD3D12Texture2DResource> DummyBlackTexture;
extern eastl::shared_ptr<FD3D12Texture2DResource> DummyWhiteTexture;

void InitDummyTexture(FD3D12CommandContext& InCommandContext);