#include "Profiling.h"

#include <windows.h>

#include "Editor/ImguiHelper.h"
#include "imgui.h"

static LARGE_INTEGER QPCFrequency{
	[]()->LARGE_INTEGER {
	LARGE_INTEGER QPCFrequency;
	QueryPerformanceFrequency(&QPCFrequency);
	return QPCFrequency;
	}() };


static struct FRegisterProfilerImguiCallback
{
	FRegisterProfilerImguiCallback()
	{
		FImguiHelperSingleton::GetInstance()->ImguiDrawEventList.emplace_back([]() {
			if (!ImGui::Begin("ReDoomsEngine", NULL, 0))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}
			else
			{
				// Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.
				// e.g. Use 2/3 of the space for widgets and 1/3 for labels (right align)
				//ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
				// e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
				ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

				const eastl::hash_map<const char* /*Timer name. Literal string*/, double /*Elapsed Seconds*/>& CPUTimerElapsedSecondsMap = FProfilingManager::GetCPUTimerElapsedSecondsMap(GCurrentFrameIndex - 1);

				auto TickTimer = CPUTimerElapsedSecondsMap.find("Tick");
				if (TickTimer != CPUTimerElapsedSecondsMap.end())
				{
					ImGui::Text("FPS : %f", 1.0 / (TickTimer->second));
					ImGui::Text("FrameTime : %f(ms)", (TickTimer->second) * 1000.0);
				}

				if (ImGui::TreeNode("CPU Timer"))
				{
					for (auto& CPUTimerElapsedSecondPair : CPUTimerElapsedSecondsMap)
					{
						ImGui::Text("%s : %f(ms)", CPUTimerElapsedSecondPair.first, CPUTimerElapsedSecondPair.second * 1000.0);
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("GPU Timer"))
				{
					ImGui::TreePop();
				}

				// End of ShowDemoWindow()
				ImGui::PopItemWidth();
				ImGui::End();
			}
			FProfilingManager::NewFrame();
		});
	}
} RegisterProfilerImguiCallback{};

eastl::hash_map<const char* /*Timer name. Literal string*/, double /*Elapsed Seconds*/> FProfilingManager::CPUTimerElapsedSecondsMap[2]{};

FCPUTimer::FCPUTimer(const char* const InTimerName) :
	TimerName(InTimerName),
	ElapsedTicks(0)
{
	LARGE_INTEGER CurrentTime;
	QueryPerformanceCounter(&CurrentTime);
	QPCLastTime = CurrentTime.QuadPart;
}

FCPUTimer::~FCPUTimer()
{
	UpdateElapsedTicks();
}

void FCPUTimer::UpdateElapsedTicks()
{
	LARGE_INTEGER CurrentTime;
	QueryPerformanceCounter(&CurrentTime);

	UINT64 TimeDelta = CurrentTime.QuadPart - QPCLastTime;

	// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
	TimeDelta *= TicksPerSecond;
	TimeDelta /= QPCFrequency.QuadPart;

	ElapsedTicks = TimeDelta;
	ElapsedSeconds = TicksToSeconds(ElapsedTicks);

	FProfilingManager::UpdateCPUTimer(this);

	QPCLastTime = CurrentTime.QuadPart;
}

void FProfilingManager::NewFrame()
{
	CPUTimerElapsedSecondsMap[GCurrentFrameIndex % 2].clear(false);
}

void FProfilingManager::UpdateCPUTimer(const FCPUTimer* const InTimer)
{
	CPUTimerElapsedSecondsMap[GCurrentFrameIndex % 2][InTimer->GetTimerName()] += InTimer->GetElapsedSeconds();
}

const eastl::hash_map<const char* /*Timer name. Literal string*/, double /*Elapsed Seconds*/>& FProfilingManager::GetCPUTimerElapsedSecondsMap(const uint64_t InFrameIndex)
{
	return CPUTimerElapsedSecondsMap[InFrameIndex % 2];
}
