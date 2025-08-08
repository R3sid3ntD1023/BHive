#pragma once

#include "ImWindowBase.h"

namespace BHive
{
	class EditorAssetManager;

	class ImAssetWindow : public ImWindowBase
	{
	public:
		ImAssetWindow();

		void SetContext(const Ref<EditorAssetManager> &manager);

		virtual void OnUpdateContent() override;

		virtual const char *GetName() const override { return "Assets "; }

	private:
		Ref<EditorAssetManager> mManagerContext;
	};

	class ImProfilerWindow : public ImWindowBase
	{
	public:
		ImProfilerWindow();

		virtual void OnUpdateContent() override;

		virtual const char *GetName() const override { return "Profiler"; }
	};
} // namespace BHive