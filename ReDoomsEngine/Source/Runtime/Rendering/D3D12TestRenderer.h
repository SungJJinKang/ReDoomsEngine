#pragma once
#include "CommonInclude.h"
#include "Renderer.h"

class D3D12TestRenderer : public FRenderer
{
public:

	virtual void Init();
	virtual void OnStartFrame();
	virtual bool Draw();
	virtual void OnEndFrame();
	virtual void Destroy();

private:

};
