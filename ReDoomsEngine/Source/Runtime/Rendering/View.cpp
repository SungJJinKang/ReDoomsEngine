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
				Vector3 EulerRot = View.Transform.Rotation.ToEuler();
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
				{
					float FovInDegree = View.FovInDegree;
					if (ImGui::DragScalar("FarPlane", ImGuiDataType_Float, &FovInDegree))
					{
						View.FovInDegree = FovInDegree;
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

Matrix FView::Get3DViewMatrices() const
{
	const Vector3& Eye = Transform.Position;
	const Vector3 Forward = Transform.Forward();
	const Vector3 Up = Transform.Up();
	return Matrix::CreateLookAt(Eye, Eye + Forward, Up);
}

Matrix FView::GetPerspectiveProjectionMatrix() const
{
	float AspectRatio = (float)ScreenWidth / (float)ScreenHeight;
	float FovAngleY = FovInDegree * XM_PI / 180.0f;

	if (AspectRatio < 1.0f)
	{
		FovAngleY /= AspectRatio;
	}

	return Matrix::CreatePerspectiveFieldOfView(FovAngleY, AspectRatio, NearPlane, FarPlane);
}

Matrix FView::GetViewPerspectiveProjectionMatrix() const
{
	return Get3DViewMatrices() * GetPerspectiveProjectionMatrix();
}

Matrix FView::GetOrthoProjMatrices() const
{
	return Matrix::CreateOrthographic(ScreenWidth, ScreenHeight, NearPlane, FarPlane);
}

void FView::Reset()
{
	Transform.Position = Vector3{ 0.0f, 50.0f, -50.0f };
	Transform.Scale = Vector3{ 1.0f, 1.0f, 1.0f };
	Transform.Rotation = Quaternion::LookRotation(Vector3::Forward, Vector3::Up);
	NearPlane = 0.01f;
	FarPlane = 12000.0f;
	FovInDegree = 90.0f;
}