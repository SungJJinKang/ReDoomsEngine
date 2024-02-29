#pragma once

#include "Vector.h"

namespace culling
{

	struct AABB
	{
		AlignedVec4 mMin;
		AlignedVec4 mMax;
	};

	struct Face
	{
		Vec3 Vertices[4];
	};

	struct AABBVertices
	{
		Face mFaces[6];
	};

	
}