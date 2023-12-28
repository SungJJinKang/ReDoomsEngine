#include "RendererManager.h"

FRendererManager::FRendererManager()
	: D3D12Manager()
{
}

void FRendererManager::Init()
{
	D3D12Manager.Init(true);
}
