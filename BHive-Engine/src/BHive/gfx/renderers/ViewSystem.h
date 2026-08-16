#pragma once

#include "core/math/Math.h"
#include "core/math/Frustum.h"

namespace BHive
{
	struct FView
	{
		glm::mat4 Projection{1.0f};
		glm::mat4 View{1.0f};
		glm::vec4 NearFar{0.0f};
		glm::vec4 Position{0.0f};

		static FView Create(const glm::mat4 &projection, const glm::mat4 &view)
		{
			FView v{};

			v.Projection = projection;
			v.View = view;
			v.NearFar.x = projection[3][2] / (projection[2][2] - 1.0f);
			v.NearFar.y = projection[3][2] / (projection[2][2] + 1.0f);
			v.Position = glm::inverse(view)[3];
			return v;
		}
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
} // namespace BHive