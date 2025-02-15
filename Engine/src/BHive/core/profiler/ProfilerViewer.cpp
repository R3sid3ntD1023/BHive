#include "ProfilerViewer.h"
#include <implot.h>

namespace BHive
{
	static ImPlotFlags plotflags = ImPlotFlags_NoMouseText;

	void ProfilerViewer::ViewFPS(const FPSCounter &counter, const float *data, size_t size)
	{

		if (ImPlot::BeginPlot("FPS", {-1, 0}, plotflags | ImPlotFlags_NoLegend))
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
		auto &datas = BHive::CPUGPUProfiler::GetInstance().GetData();

		if (ImPlot::BeginPlot("Profiler", {-1, 0}, plotflags))
		{
			ImPlot::SetupAxes(nullptr, "Elasped Time (s)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_LockMin);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1, ImGuiCond_Once);
			ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside | ImPlotLegendFlags_Sort);

			for (const auto &[name, data] : datas)
			{
				auto &samples = data.GetSamples();

				ImPlot::PlotLine(name.c_str(), samples.data(), samples.size());
			}
			ImPlot::EndPlot();
		}
	}
} // namespace BHive
