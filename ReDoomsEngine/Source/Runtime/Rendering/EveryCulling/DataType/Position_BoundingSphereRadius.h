#pragma once

#include "../EveryCullingCore.h"
#include "Math/Vector.h"

namespace culling
{
	struct alignas(16) Position_BoundingSphereRadius
	{

		culling::Vec4 Position;


		EASTL_FORCE_INLINE void SetPosition(const culling::Vec3& _position)
		{
			EA::StdC::Memcpy(Position.data(), _position.data(), sizeof(culling::Vec3));
		}

		EASTL_FORCE_INLINE const culling::Vec3& GetPosition() const
		{
			return *reinterpret_cast<const culling::Vec3*>(Position.data());
		}

		EASTL_FORCE_INLINE void SetPosition(const float* const vec3)
		{
			EA::StdC::Memcpy(Position.data(), vec3, sizeof(culling::Vec3));
		}

		EASTL_FORCE_INLINE void SetBoundingSphereRadius(const float boundingSphereRadius)
		{
			// why minus?
			// Calculated distance between frustum plane and object is positive when object is in frustum
			EA_ASSERT(boundingSphereRadius > 0.0f);
			Position.values[3] = boundingSphereRadius;
		}

		EASTL_FORCE_INLINE float GetBoundingSphereRadius() const
		{
			return Position.values[3];
		}
	};
}
