#include "DetailsPanel.h"
#include "inspector/Inspectors.h"
#include "gui/ImGuiExtended.h"
#include "core/subsystem/SubSystem.h"
#include "subsystems/EditSubSystem.h"
#include "scene/ObjectBase.h"

namespace BHive
{
	void DetailsPanel::OnGuiRender()
	{
		if (mGetActiveObject)
		{
			auto object = mGetActiveObject();

			if (object)
			{
				rttr::variant obj_var = object;
				if (inspect(obj_var))
				{
					object = obj_var.get_value<ObjectBase *>();
				}
			}
		}
	}
} // namespace BHive