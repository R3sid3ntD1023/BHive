#include "InspectorCore.h"
#include <imgui_internal.h>

namespace BHive::inspectors
{
	void SetImGuiContext(ImGuiContext *context)
	{
		ImGui::SetCurrentContext(context);
	}
} // namespace BHive::inspectors