#pragma once
struct FD3D12CommandContext;
class ID3D12RendererStateCallbackInterface
{
public:
	virtual ~ID3D12RendererStateCallbackInterface() = default;
	virtual void OnPreStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext) = 0;
	virtual void OnPreEndFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext) = 0;
	virtual void OnPostEndFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnDestory(FD3D12CommandContext& InCommandContext);
};