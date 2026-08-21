#include "PerformanceLayer.h"
#include "core/Application.h"
#include "core/FPSCounter.h"

namespace BHive
{
	void PerformanceLayer::OnGuiRender()
	{
		float fps = FPSCounter::Get();

		if (ImGui::Begin("FPS"))
		{
			ImGui::Text("CPU FPS: %.2f", fps);
		}

		ImGui::End();
	}
} // namespace BHive