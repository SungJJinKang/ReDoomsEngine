#include "Common/RendererStateCallbackInterface.h"

void IRendererStateCallbackInterface::OnPreStartFrame()
{
}

void IRendererStateCallbackInterface::OnPreEndFrame(FD3D12CommandContext& InCommandContext)
{

}

void IRendererStateCallbackInterface::OnPostEndFrame()
{
}

void IRendererStateCallbackInterface::OnDestory(FD3D12CommandContext& InCommandContext)
{

}
