#include "ProfilerViewer.h"
#include <implot.h>

namespace BHive
{
	static ImPlotFlags plotflags = ImPlotFlags_NoMouseText;

	void ProfilerViewer::ViewFPS(const FPSCounter &counter, const float *data, size_t size)
	{

		if (ImPlot::BeginPlot("FPS", {-1, 0}, plotflags))
		{
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1000, ImGuiCond_Once);

			auto fps_overlay = std::format("FPS: {:.2f}", (float)counter);
			ImPlot::SetupAxes(nullptr, fps_overlay.c_str(), ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_LockMin);
			ImPlot::PlotLine("FPS", data, size);

			ImPlot::EndPlot();
		}
	}
	void ProfilerViewer::ViewCPUGPU()
	{
		if (ImPlot::BeginPlot("Profiler", {-1, 0}, plotflags))
		{
			ImPlot::SetupAxes(nullptr, "Elasped Time (s)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_LockMin);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1, ImGuiCond_Once);

			for (const auto &[name, data] : BHive::CPUGPUProfiler::GetInstance().GetData())
			{
				auto &samples = data.GetSamples();

				ImPlot::PlotLine(name.c_str(), samples.data(), samples.size());
			}
			ImPlot::EndPlot();
		}
	}
} // namespace BHive
