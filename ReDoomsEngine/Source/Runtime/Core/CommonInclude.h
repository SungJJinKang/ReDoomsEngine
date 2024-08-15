#pragma once

#ifndef UNICODE
#error "ReDoomsEngine requires unicode"
#endif

#include "Macros.h"
#include "Types.h"
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
#include "DirectXMath.h"
#include "DirectXCollision.h"
#include "DirectXColors.h"
#include "DirectXPackedVector.h"
#include "Math/SimpleMath.h"
#include "Math/MathUtils.h"
using namespace DirectX::SimpleMath;
using namespace DirectX;

#include "Log.h"
#include "TextHelper.h"
#include "Utils.h"
#include "Profiling.h"
#include "Thread.h"

#include "Utils/ConsoleVariable.h"
