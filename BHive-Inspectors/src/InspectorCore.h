#pragma once

#include "core/Core.h"

struct ImGuiContext;

namespace BHive::inspectors
{
	BHIVE_API void SetImGuiContext(ImGuiContext *context);
}