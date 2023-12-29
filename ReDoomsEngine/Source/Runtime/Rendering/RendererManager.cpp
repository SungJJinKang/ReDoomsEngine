#include "RendererManager.h"

FRendererManager::FRendererManager()
	: 
	D3D12Manager(true)
{
}

void FRendererManager::Init()
{
	D3D12Manager.Init();

}

void FRendererManager::Draw()
{
}

void FRendererManager::Destroy()
{
}
