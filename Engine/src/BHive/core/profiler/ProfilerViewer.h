#pragma once

#include "CPUGPUProfiler.h"
#include "core/FPSCounter.h"

namespace BHive
{
	struct BHIVE_API ProfilerViewer
	{
		static void ViewFPS();
		static void ViewCPUGPU();
	};
} // namespace BHive
