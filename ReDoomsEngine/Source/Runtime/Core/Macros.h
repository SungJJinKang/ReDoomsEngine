#pragma once

#if defined(_DEBUG)
#define RD_DEBUG 1
#define RD_ASSUME(Condition) EA_ASSERT(Condition); EA_ASSUME(Condition);
#else
#define RD_DEBUG 0
#define RD_ASSUME(Condition) EA_ASSUME(Condition);
#endif

