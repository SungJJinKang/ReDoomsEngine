#include "ConsoleVariable.h"

#include "CommonInclude.h"
#include "Editor/imguiHelper.h"
#include "imgui.h"

// static TConsoleVariable<bool> GTest0{ "r.TestBool", true };
// static TConsoleVariable<int32_t> GTest1{ "r.TestInt32", 232233123 };
// static TConsoleVariable<uint32_t> GTest2{ "r.TestUInt32", -51313 };
// static TConsoleVariable<int64_t> GTest3{ "r.TestInt64", 11245 };
// static TConsoleVariable<uint64_t> GTest4{ "r.TestUInt64", -32212 };
// static TConsoleVariable<float> GTest5{ "r.TestFloat", 10.0f };
// static TConsoleVariable<double> GTest6{ "r.TestDouble", 50.0 };

static struct FRegisterConsoleVariableImguiCallback
{
	FRegisterConsoleVariableImguiCallback()
	{
		FImguiHelperSingleton::GetInstance()->ImguiDrawEventList.emplace_back([]() {
			if (!ImGui::Begin("ConsoleVariable", NULL, 0))
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
				// e.g. Leave a fixed amount of width for labels (by passing a negative Delta), the rest goes to widgets.
				ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

				const eastl::hash_map<const char*, FConsoleVariable*>& ConsoleVariables
					= FConsoleVariableManagerSingleton::GetInstance()->GetConsoleVariables();

				for (auto& ConsoleVariable : ConsoleVariables)
				{
					if (!(ConsoleVariable.second->GetConsoleVariableFlag() & EConsoleVariableFlag::HideInEditor))
					{
						if (ConsoleVariable.second->IsBoolean())
						{
							bool bIsChecked = ConsoleVariable.second->GetBool();
							if (ImGui::Checkbox(ConsoleVariable.first, &bIsChecked))
							{
								ConsoleVariable.second->SetBool(bIsChecked);
							}
						}
						else if (ConsoleVariable.second->IsInt32())
						{
							int32_t Value = ConsoleVariable.second->GetInt();
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_S32, &Value))
							{
								ConsoleVariable.second->SetInt(Value);
							}
						}
						else if (ConsoleVariable.second->IsUInt32())
						{
							uint32_t Value = ConsoleVariable.second->GetUInt();
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_U32, &Value))
							{
								ConsoleVariable.second->SetUInt(Value);
							}
						}
						else if (ConsoleVariable.second->IsInt64())
						{
							int64_t Value = ConsoleVariable.second->GetInt64();
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_S64, &Value))
							{
								ConsoleVariable.second->SetInt64(Value);
							}
						}
						else if (ConsoleVariable.second->IsUInt64())
						{
							uint64_t Value = ConsoleVariable.second->GetUInt64();
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_U64, &Value))
							{
								ConsoleVariable.second->SetUInt64(Value);
							}
						}
						else if (ConsoleVariable.second->IsFloat())
						{
							float Value = ConsoleVariable.second->GetFloat();
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_Float, &Value))
							{
								ConsoleVariable.second->SetFloat(Value);
							}
						}
						else if (ConsoleVariable.second->IsDouble())
						{
							double Value = ConsoleVariable.second->GetDouble();
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_Double, &Value))
							{
								ConsoleVariable.second->SetDouble(Value);
							}
						}
						else
						{
							EA_ASSERT_MSG(false, "Unsupported Type");
						}
					}
				}

				ImGui::PopItemWidth();
				ImGui::End();
			}
			});
	}
} RegisterConsoleVariableImguiCallback{};

void FConsoleVariableManager::AddConsoleVariable(FConsoleVariable* const InConsoleVariable)
{
	EA_ASSERT(ConsoleVariables.find(InConsoleVariable->GetName()) == ConsoleVariables.end());
	ConsoleVariables.emplace(InConsoleVariable->GetName(), InConsoleVariable);
}

FConsoleVariable* FConsoleVariableManager::GetConsoleVariable(const char* const InName)
{
	return ConsoleVariables.at(InName);
}

const FConsoleVariable* FConsoleVariableManager::GetConsoleVariable(const char* const InName) const
{
	return ConsoleVariables.at(InName);
}

const eastl::hash_map<const char*, FConsoleVariable*>& FConsoleVariableManager::GetConsoleVariables() const
{
	return ConsoleVariables;
}

FConsoleVariable::FConsoleVariable(const char* const InName, const EConsoleVariableFlag InConsoleVariableFlag) 
	: Name(InName), ConsoleVariableFlag(InConsoleVariableFlag)
{
	FConsoleVariableManagerSingleton::GetInstance()->AddConsoleVariable(this);
}
