#include "HistoryWindow.h"
#include "core/subsystem/SubSystem.h"
#include "undoredo/UndoRedo.h"
#include <imgui.h>

namespace BHive
{
	void HistoryWindow::OnGuiRender()
	{
		auto &undo_system = GetSubSystem<UndoRedo>();
		const auto &count = undo_system.get_command_count();
		const auto &index = undo_system.get_current_command_index();

		for (uint16_t i = 0; i < count; i++)
		{
			const auto &command = undo_system.get_command_at(i);

			ImGui::PushID(&command);
			ImGui::Selectable(command.Name.c_str(), index == i);
			ImGui::PopID();
		}

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Clear"))
			{
				undo_system.clear();
			}
			ImGui::EndPopup();
		}
	}
} // namespace BHive