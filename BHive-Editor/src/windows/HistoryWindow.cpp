#include "HistoryWindow.h"
#include "core/subsystem/SubSystem.h"
#include "undoredo/UndoRedo.h"
#include <imgui.h>

namespace BHive
{
	void HistoryWindow::OnGuiRender()
	{
		const auto &undo_system = GetSubSystem<UndoRedo>();
		const auto &count = undo_system.get_command_count();
		const auto &index = undo_system.get_current_command_index();

		for (uint16_t i = 0; i < count; i++)
		{
			const auto &command = undo_system.get_command_at(i);

			ImGui::Selectable(command.Name.c_str(), index == i);
		}
	}
} // namespace BHive