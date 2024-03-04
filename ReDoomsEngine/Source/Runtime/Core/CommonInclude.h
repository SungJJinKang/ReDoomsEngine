#pragma once

#ifndef UNICODE
#error "ReDoomsEngine requires unicode"
#endif

#include "Macros.h"
#include "MemoryCore.h"

#include "combaseapi.h"

#include "wrl/client.h"
using Microsoft::WRL::ComPtr;

// STL
#include "EABase/eabase.h"
#include "EAAssert/eaassert.h"
#include "EABase/earesult.h"
#include "EAStdC/EASprintf.h"
#include "EAStdC/EAString.h"
#include "EAStdC/EATextUtil.h"
#include "EAStdC/EACType.h"
#include "EASTL/array.h"
#include "EASTL/string.h"
#include "EASTL/fixed_string.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/vector.h"
#include "EASTL/vector_map.h"
#include "EASTL/vector_set.h"
#include "EASTL/hash_map.h"
#include "EASTL/unique_ptr.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/bitvector.h"
#include "EASTL/queue.h"
#include "EASTL/optional.h"
#include "EAStdC/EASingleton.h"
#include "eathread/eathread_futex.h"
#include "eathread/eathread_thread.h"

// Math
#include "Math/JINMATH/Matrix2x2.h"
#include "Math/JINMATH/Matrix3x3.h"
#include "Math/JINMATH/Matrix4x4.h"
#include "Math/JINMATH/Matrix_utility.h"
#include "Math/JINMATH/Quaternion.h"
#include "Math/JINMATH/Vector2.h"
#include "Math/JINMATH/Vector3.h"
#include "Math/JINMATH/Vector4.h"
#include "Math/MathUtils.h"

#include "Log.h"
#include "TextHelper.h"
#include "Utils.h"
#include "Profiling.h"
#include "Thread.h"

#include "Utils/ConsoleVariable.h"
