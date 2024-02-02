#pragma once
#include "CommonInclude.h"
#include "D3D12Manager.h"
#include "D3D12Window.h"
#include "D3D12RendererInclude.h"

class FRenderer : public EA::StdC::Singleton<FRenderer>
{
public:

	FRenderer();
	virtual void Init();
	virtual void OnStartFrame();
	virtual bool Draw();
	virtual void OnEndFrame();
	virtual void Destroy();

	FD3D12Manager D3D12Manager;

protected:

	FD3D12CommandContext CurrentFrameCommandContext;

	eastl::unique_ptr<FD3D12BufferResource> VertexBuffer;
	uint32_t VerticeStride;

};

