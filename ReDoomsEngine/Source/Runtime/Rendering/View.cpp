#include "View.h"

#include "Editor/imguiHelper.h"
#include "imgui.h"

#include "Renderer/Renderer.h"

static struct FRegisterViewCallback 
{
	FRegisterViewCallback() 
	{
		FImguiHelperSingleton::GetInstance()->ImguiDrawEventList.emplace_back([]() { 
			if (!ImGui::Begin("View", 0, 0)) 
			{
				ImGui::End(); return;
			}
			else 
			{
				ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

				FView& View = FRenderer::GetInstance()->View;
				ImGui::Text("Position : %f %f %f", View.Transform.Position.x, View.Transform.Position.y, View.Transform.Position.z); 
				math::Vector3 EulerRot = View.Transform.Rotation.ToEulerAngle();
				ImGui::Text("Rotation : %f %f %f", EulerRot.x, EulerRot.y, EulerRot.z);
				
				{
					float NearPlane = View.NearPlane;
					if (ImGui::DragScalar("NearPlane", ImGuiDataType_Float, &NearPlane))
					{
						View.NearPlane = NearPlane;
					}
				}

				{
					float FarPlane = View.FarPlane;
					if (ImGui::DragScalar("FarPlane", ImGuiDataType_Float, &FarPlane))
					{
						View.FarPlane = FarPlane;
					}
				}

				ImGui::PopItemWidth(); 
				ImGui::End();
			} 
		});
	}
} RegisterViewCallback{};

FView::FView()
{
	Reset();
}

math::Matrix4x4 FView::Get3DViewMatrices() const
{
	const math::Vector3& Eye = Transform.Position;
	const math::Vector3 Forward = Transform.Forward();
	const math::Vector3 Up = Transform.Up();
	return math::lookAt(Eye, Eye + Forward, Up);
}

math::Matrix4x4 FView::GetPerspectiveProjectionMatrix() const
{
	return math::perspectiveFov(FovInDegree * DEGREE_TO_RADIAN, ScreenHeight, NearPlane, NearPlane, FarPlane);
}

math::Matrix4x4 FView::GetViewPerspectiveProjectionMatrix() const
{
	return GetPerspectiveProjectionMatrix() * Get3DViewMatrices();
}

math::Matrix4x4 FView::GetOrthoProjMatrices() const
{
	return math::ortho(ScreenWidth, ScreenHeight, NearPlane, FarPlane);
}

void FView::Reset()
{
	Transform.Position = math::Vector3{ 0.0f, 50.0f, -50.0f };
	Transform.Scale = math::Vector3{ 1.0f, 1.0f, 1.0f };
	Transform.Rotation = math::Quaternion::LookAt(math::Vector3::forward, math::Vector3::up);
	NearPlane = 0.01f;
	FarPlane = 12000.0f;
	FovInDegree = 90.0f;
}