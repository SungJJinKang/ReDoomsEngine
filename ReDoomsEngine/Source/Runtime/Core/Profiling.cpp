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

				if (ImGui::TreeNode("CPU Timer"))
				{
					for (auto& CPUTimerElapsedSecondPair : FProfilingManager::GetCPUTimerElapsedSecondsMap())
					{
						ImGui::Text("%s : %f(s)", CPUTimerElapsedSecondPair.first, CPUTimerElapsedSecondPair.second);
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
			}}
		);
	}
} RegisterProfilerImguiCallback{};

eastl::hash_map<const char* /*Timer name. Literal string*/, double /*Elapsed Seconds*/> FProfilingManager::CPUTimerElapsedSecondsMap{};

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
	LARGE_INTEGER CurrentTime;
	QueryPerformanceCounter(&CurrentTime);

	UINT64 TimeDelta = CurrentTime.QuadPart - QPCLastTime;

	// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
	TimeDelta *= TicksPerSecond;
	TimeDelta /= QPCFrequency.QuadPart;

	ElapsedTicks = TimeDelta;

	FProfilingManager::UpdateCPUTimer(this);
}

void FProfilingManager::UpdateCPUTimer(const FCPUTimer* const InTimer)
{
	CPUTimerElapsedSecondsMap[InTimer->GetTimerName()] = InTimer->GetElapsedSeconds();
}

const eastl::hash_map<const char* /*Timer name. Literal string*/, double /*Elapsed Seconds*/>& FProfilingManager::GetCPUTimerElapsedSecondsMap()
{
	return CPUTimerElapsedSecondsMap;
}
