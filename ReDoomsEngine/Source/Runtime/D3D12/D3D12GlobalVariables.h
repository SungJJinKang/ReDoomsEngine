#pragma once
#include "CommonInclude.h"

#include "D3D12Enums.h"

inline constexpr uint32_t GNumBackBufferCount = 3;
extern uint64_t GCurrentFrameIndex;
extern uint64_t GCurrentBackbufferIndex;
extern float GTimeDelta;
inline constexpr int32_t GWindowWidth = 1024;
inline constexpr int32_t GWindowHeight = 720;
extern ERendererState GCurrentRendererState;