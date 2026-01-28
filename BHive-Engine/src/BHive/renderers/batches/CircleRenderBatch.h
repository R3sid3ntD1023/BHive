#pragma once

#include "TRenderBatch.h"

namespace BHive
{
	struct CircleVertex
	{
		glm::vec4 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;
		int32_t EntityID{-1};

		static BufferLayout GetLayout();
	};

	struct CircleRenderBatch : public TRenderBatch<CircleVertex>
	{

		Ref<Shader> GetShader() const override;

		void Flush() override;
	};
} // namespace BHive