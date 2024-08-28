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
							TConsoleVariable<bool>* TemplateConsoleVar = static_cast<TConsoleVariable<bool>*>(ConsoleVariable.second);
							bool bIsChecked = *TemplateConsoleVar;
							if (ImGui::Checkbox(ConsoleVariable.first, &bIsChecked))
							{
								*TemplateConsoleVar = bIsChecked;
							}
						}
						else if (ConsoleVariable.second->IsInt32())
						{
							TConsoleVariable<int32>* TemplateConsoleVar = static_cast<TConsoleVariable<int32>*>(ConsoleVariable.second);
							int32_t Value = *TemplateConsoleVar;
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_S32, &Value))
							{
								*TemplateConsoleVar = Value;
							}
						}
						else if (ConsoleVariable.second->IsUInt32())
						{
							TConsoleVariable<uint32>* TemplateConsoleVar = static_cast<TConsoleVariable<uint32>*>(ConsoleVariable.second);
							uint32_t Value = *TemplateConsoleVar;
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_U32, &Value))
							{
								*TemplateConsoleVar = Value;
							}
						}
						else if (ConsoleVariable.second->IsInt64())
						{
							TConsoleVariable<int64>* TemplateConsoleVar = static_cast<TConsoleVariable<int64>*>(ConsoleVariable.second);
							int64_t Value = *TemplateConsoleVar;
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_S64, &Value))
							{
								*TemplateConsoleVar = Value;
							}
						}
						else if (ConsoleVariable.second->IsUInt64())
						{
							TConsoleVariable<uint64>* TemplateConsoleVar = static_cast<TConsoleVariable<uint64>*>(ConsoleVariable.second);
							uint64_t Value = *TemplateConsoleVar;
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_U64, &Value))
							{
								*TemplateConsoleVar = Value;
							}
						}
						else if (ConsoleVariable.second->IsFloat())
						{
							TConsoleVariable<float>* TemplateConsoleVar = static_cast<TConsoleVariable<float>*>(ConsoleVariable.second);
							float Value = *TemplateConsoleVar;
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_Float, &Value))
							{
								*TemplateConsoleVar = Value;
							}
						}
						else if (ConsoleVariable.second->IsDouble())
						{
							TConsoleVariable<double>* TemplateConsoleVar = static_cast<TConsoleVariable<double>*>(ConsoleVariable.second);
							double Value = *TemplateConsoleVar;
							if (ImGui::DragScalar(ConsoleVariable.first, ImGuiDataType_Double, &Value))
							{
								*TemplateConsoleVar = Value;
							}
						}
						else if (ConsoleVariable.second->IsVector2())
						{
							TConsoleVariable<Vector2>* TemplateConsoleVar = static_cast<TConsoleVariable<Vector2>*>(ConsoleVariable.second);
							Vector2 Value = *TemplateConsoleVar;
							if (ImGui::DragScalarN(ConsoleVariable.first, ImGuiDataType_Float, &Value, 2))
							{
								*TemplateConsoleVar = Value;
							}
						}
						else if (ConsoleVariable.second->IsVector3())
						{
							TConsoleVariable<Vector3>* TemplateConsoleVar = static_cast<TConsoleVariable<Vector3>*>(ConsoleVariable.second);
							Vector3 Value = *TemplateConsoleVar;
							if (ImGui::DragScalarN(ConsoleVariable.first, ImGuiDataType_Float, &Value, 3))
							{
								*TemplateConsoleVar = Value;
							}
						}
						else if (ConsoleVariable.second->IsVector4())
						{
							TConsoleVariable<Vector4>* TemplateConsoleVar = static_cast<TConsoleVariable<Vector4>*>(ConsoleVariable.second);
							Vector4 Value = *TemplateConsoleVar;
							if (ImGui::DragScalarN(ConsoleVariable.first, ImGuiDataType_Float, &Value, 4))
							{
								*TemplateConsoleVar = Value;
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
