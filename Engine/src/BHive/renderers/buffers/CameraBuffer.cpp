#include "CameraBuffer.h"
#include "gfx/UniformBuffer.h"
#include "GlobalBuffers.h"

namespace BHive
{
	void CameraBuffer::Init()
	{
		mBuffer = UniformBuffer::Create(0, sizeof(FCameraData));
		GlobalBuffers::AddGlobalUniformBuffer(0, mBuffer);
	}

	void CameraBuffer::Begin(const glm::mat4 &proj, const glm::mat4 &view)
	{
		ASSERT(mBuffer, "Did you call init()?");

		mData.Projection = proj;
		mData.View = view;
		mData.NearFar.x = proj[3][2] / (proj[2][2] - 1.0f);
		mData.NearFar.y = proj[3][2] / (proj[2][2] + 1.0f);
		mData.Position = glm::inverse(view)[3];

		mBuffer->SetData(&mData, sizeof(FCameraData));

		mViewFrustum.Update(proj, view);
	}
} // namespace BHive