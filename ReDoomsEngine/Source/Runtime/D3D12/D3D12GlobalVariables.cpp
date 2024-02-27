#include "D3D12GlobalVariables.h"

uint64_t GCurrentFrameIndex = 0;
uint64_t GCurrentBackbufferIndex = 0;
float GTimeDelta = 0.0f;
ERendererState GCurrentRendererState = ERendererState::Initializing;
