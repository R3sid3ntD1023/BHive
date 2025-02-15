#pragma once

#include "CPUGPUProfiler.h"
#include "core/FPSCounter.h"

namespace BHive
{
	struct ProfilerViewer
	{
		static void ViewFPS(const FPSCounter &counter, const float *data, size_t size);
		static void ViewCPUGPU();
	};
} // namespace BHive
