#pragma once

#include "CommonInclude.h"
#include "D3D12RendererStateCallbackInterface.h"
#include "D3D12Descriptor.h"

class FImguiHelper
{
public:

	void Init();
	
	void NewFrame();
	void EndDraw(FD3D12CommandContext& InCommandContext);
	void OnDestory();

	eastl::vector<eastl::function<void()>> ImguiDrawEventList;

private:

	FD3D12DescriptorHeapBlock SRVDescriptorHeapBlock;

};

class FImguiHelperSingleton : public EA::StdC::SingletonAdapter<FImguiHelper, TRUE> { };