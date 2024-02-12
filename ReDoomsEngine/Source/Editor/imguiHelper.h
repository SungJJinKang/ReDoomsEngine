#pragma once

#include "CommonInclude.h"
#include "D3D12RendererStateCallbackInterface.h"
#include "D3D12Descriptor.h"

class FimguiHelper
{
public:

	static void Init();
	
	static void NewFrame();
	static void EndDraw(FD3D12CommandContext& InCommandContext);
	static void OnDestory();

	static eastl::vector<eastl::function<void()>> imguiDrawEventList;

private:

	static FD3D12DescriptorHeapBlock SRVDescriptorHeapBlock;

};

