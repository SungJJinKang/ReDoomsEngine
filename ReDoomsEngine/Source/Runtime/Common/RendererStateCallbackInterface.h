#pragma once
struct FD3D12CommandContext;
class IRendererStateCallbackInterface
{
public:
	virtual ~IRendererStateCallbackInterface() = default;
	virtual void OnPreStartFrame();
	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext) = 0;
	virtual void OnPreEndFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext) = 0;
	virtual void OnPostEndFrame();
	virtual void OnDestory(FD3D12CommandContext& InCommandContext);
};