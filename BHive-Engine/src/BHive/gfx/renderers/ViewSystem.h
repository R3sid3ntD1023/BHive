#pragma once

#include "core/math/Math.h"

namespace BHive
{
	struct FView
	{
		glm::mat4 Projection{1.0f};
		glm::mat4 View{1.0f};
		glm::vec4 NearFar{0.0f};
		glm::vec4 Position{0.0f};
	};

	class ViewSystem
	{
	public:
		ViewSystem() = default;

		void BeginFrame();
		void EndFrame();

		FView &CreateMainView();
		const FView &GetMainView() const;

		const std::vector<FView> &GetAllViews() const { return mViews; }

	private:
		std::vector<FView> mViews;
	};
}