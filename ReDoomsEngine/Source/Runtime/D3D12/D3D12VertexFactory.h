#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

enum EVertexFactory
{
	SinglePrimitive,
	InstancedDraw,
	Num
};

eastl::vector<eastl::string> GetAdditionalDefinesForVertexFactory(const EVertexFactory InVertexFactory);
