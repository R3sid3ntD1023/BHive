#pragma once

#include "CPUGPUProfiler.h"
#include "core/FPSCounter.h"

namespace BHive
{
	struct ProfilerViewer
	{
		static void ViewFPS();
		static void ViewCPUGPU();
	};
} // namespace BHive
