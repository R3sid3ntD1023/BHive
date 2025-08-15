#include "HistoryWindow.h"
#include "core/subsystem/SubSystem.h"
#include "undoredo/UndoRedo.h"
#include <imgui.h>

namespace BHive
{
	ImHistoryWindow::ImHistoryWindow()
		: ImWindowBase(ImGuiWindowFlags_NoSavedSettings)
	{
	}

	void ImHistoryWindow::OnUpdateContent()
	{
		auto &undo_system = GetSubSystem<UndoRedo>();
		const auto &index = undo_system.GetCurrentCommandIndex();

		uint16_t i = 0;
		for (const auto &it : undo_system)
		{

			ImGui::PushID(it.Command.get());
			ImGui::Selectable(it.Name.c_str(), index == i++);
			ImGui::PopID();
		}

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Clear"))
			{
				undo_system.Clear();
			}
			ImGui::EndPopup();
		}
	}
} // namespace BHive