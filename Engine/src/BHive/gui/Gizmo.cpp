#include "Gimzo.h"
#include <imgui.h>
#include <ImGuizmo.h>

namespace BHive
{
	void GizmoContext::SetOrthographic(bool ortho)
	{
		ImGuizmo::SetOrthographic(ortho);
	}

	void GizmoContext::SetRect(float x, float y, float w, float h)
	{
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		ImGuizmo::SetRect(x, y, w, h);
	}

	bool GizmoContext::IsUsing()
	{
		return ImGuizmo::IsUsingAny();
	}

	void ViewGizmo::ViewManipulate(glm::mat4 &view, float distance, const glm::vec2 &pos, const glm::vec2 &size, int32_t bg_color)
	{
		ImGuizmo::ViewManipulate(&view[0][0], distance, {pos.x, pos.y}, {size.x, size.y}, bg_color);
	}

	bool ViewGizmo::IsUsing() const
	{
		return ImGuizmo::IsUsingViewManipulate();
	}

	bool Gizmo::Manipulate(const glm::mat4 &view, const glm::mat4 &projection, glm::mat4 &transform, glm::mat4 &delta, const glm::vec3 &snapping, float *bounds)
	{
		return ImGuizmo::Manipulate(&view[0][0], &projection[0][0], (ImGuizmo::OPERATION)mOperation, (ImGuizmo::MODE)mMode, &transform[0][0], &delta[0][0], &snapping[0], bounds);
	}

	void Gizmo::SetOperationAndMode(int32_t operation, int32_t mode)
	{
		mOperation = operation;
		mMode = mode;
	}

	bool Gizmo::IsUsing() const
	{
		return ImGuizmo::IsUsing();
	}

} // namespace BHive