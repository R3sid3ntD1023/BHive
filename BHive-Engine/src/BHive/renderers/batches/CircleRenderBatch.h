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

	class IMaterialBackendInterface;

	struct CircleRenderBatch : public TRenderBatch<CircleVertex>
	{
		~CircleRenderBatch();

		void Init(size_t vcount, size_t icount) override;

		Ref<Pipeline> GetPipeline() const override;

		void Flush() override;

	private:
		Ref<Pipeline> mPipeline;

		Ref<IMaterialBackendInterface> mMaterial;

	};
} // namespace BHive