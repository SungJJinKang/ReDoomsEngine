#include "D3D12VertexFactory.h"

eastl::vector<eastl::string> GetAdditionalDefinesForVertexFactory(const EVertexFactory InVertexFactory)
{
	eastl::vector<eastl::string> AdditionalDefines;
	switch (InVertexFactory)
	{
	case EVertexFactory::SinglePrimitive:

		break;
	case EVertexFactory::InstancedDraw:
		AdditionalDefines.emplace_back("INSTANCED_DRAW");
		break;
	default:
		break;
	}
	return AdditionalDefines;
}
