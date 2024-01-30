#pragma once
#include "CommonInclude.h"
#include "D3D12Manager.h"
#include "D3D12Window.h"

class FRendererManager : public EA::StdC::Singleton<FRendererManager>
{
public:

	FRendererManager();
	void Init();
	void Draw();
	void Destroy();

	FD3D12Manager D3D12Manager;

private:


};

