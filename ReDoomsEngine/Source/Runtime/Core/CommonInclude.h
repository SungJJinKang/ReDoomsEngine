#pragma once

#include "MemoryCore.h"

#include "combaseapi.h"

#include "wrl/client.h"
using namespace Microsoft::WRL;

// EASTL

#include "EABase/eabase.h"
#include "EAAssert/eaassert.h"
#include "EABase/earesult.h"
#include "EAStdC/EASingleton.h"
#include "EAStdC/EAMemory.h"
#include "EAStdC/EASprintf.h"
#include "EAStdC/EATextUtil.h"
#include "EAStdC/EACType.h"
#include "EASTL/string.h"
#include "EASTL/fixed_string.h"
#include "EASTL/vector.h"
#include "EASTL/vector_map.h"
#include "EASTL/vector_set.h"
#include "EASTL/unique_ptr.h"
#include "EASTL/shared_ptr.h"
#include "eathread/eathread_futex.h"

#include "Log.h"