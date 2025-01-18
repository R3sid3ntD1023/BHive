#include "Animator/BlackBoard.h"
#include "gui/ImGuiExtended.h"
#include "InspectorBlackBoard.h"
#include "Inspectors.h"

namespace BHive
{
	struct FBlackBoardOperation
	{
		virtual void execute(BlackBoard &blackboard) = 0;
	};

	struct FAddKeyOperation : FBlackBoardOperation
	{
		FAddKeyOperation(const std::string &name, const Ref<BlackBoardKey> &key)
			: mName(name),
			  mKey(key)
		{
		}
		virtual void execute(BlackBoard &blackboard) override { blackboard.AddKey(mName, mKey); }

	private:
		std::string mName;
		Ref<BlackBoardKey> mKey;
	};

	bool Inspector_BlackBoard::Inspect(
		rttr::variant &var, bool read_only, const meta_getter &get_metadat)
	{
		auto data = var.get_value<BlackBoard>();

		if (read_only)
			return false;

		bool changed = false;

		FBlackBoardOperation *mOperation = nullptr;

		if (ImGui::BeginTable("##Keys", 2, ImGuiTableFlags_BordersV))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			if (ImGui::Button("Add Key"))
			{
				ImGui::OpenPopup("BlackBoardKeys");
			}

			if (ImGui::BeginPopup("BlackBoardKeys"))
			{
				auto derived_types = rttr::type::get<BlackBoardKey>().get_derived_classes();
				for (auto &type : derived_types)
				{
					if (ImGui::MenuItem(type.get_name().data()))
					{
						auto key = type.create().get_value<Ref<BlackBoardKey>>();
						changed |= true;
						mOperation = new FAddKeyOperation("NewKey", key);
					}
				}

				ImGui::EndPopup();
			}

			changed |= inspect("Keys", data.GetKeys());

			ImGui::EndTable();
		}

		if (changed)
		{
			if (mOperation)
			{
				mOperation->execute(data);
				delete mOperation;
			}

			var = data;
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_BlackBoard, BlackBoard);
} // namespace BHive