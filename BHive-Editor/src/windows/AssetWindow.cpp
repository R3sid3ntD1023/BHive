#include "AssetWindow.h"
#include "asset/EditorAssetManager.h"
#include "Inspectors/Inspect.h"
#include "core/profiler/ProfilerViewer.h"

namespace BHive
{
	ImAssetWindow::ImAssetWindow()
		: ImWindowBase(ImGuiWindowFlags_NoSavedSettings)
	{
	}

	void ImAssetWindow::SetContext(const Ref<EditorAssetManager> &manager)
	{
		mManagerContext = manager;
	}

	void ImAssetWindow::OnUpdateContent()
	{
		if (!mManagerContext)
			return;

		const auto &registry = mManagerContext->GetAssetRegistry();
		Inspect::get().inspect("", mManagerContext.get(), registry);
	}

	ImProfilerWindow::ImProfilerWindow()
		: ImWindowBase(ImGuiWindowFlags_NoSavedSettings)
	{
	}

	void ImProfilerWindow::OnUpdateContent()
	{
		ProfilerViewer::ViewFPS();
		ProfilerViewer::ViewCPUGPU();
	}
} // namespace BHive