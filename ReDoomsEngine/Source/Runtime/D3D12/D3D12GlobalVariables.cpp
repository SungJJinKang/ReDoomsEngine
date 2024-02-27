#include "D3D12GlobalVariables.h"

uint64_t GCurrentFrameIndex = 0;
uint64_t GCurrentBackbufferIndex = 0;
uint64_t GPreviousFrameDrawCallCount = 0;
uint64_t GDrawCallCount = 0;
float GTimeDelta = 0.0f;
ERendererState GCurrentRendererState = ERendererState::Initializing;

void ResetD3D12GlobalVariablesForNewFrame()
{
	GPreviousFrameDrawCallCount = GDrawCallCount;
	GDrawCallCount = 0;
}
