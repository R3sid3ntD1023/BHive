// #pragma once
//
// #include "core/Core.h"
// #include "core/math/Frustum.h"
//
// namespace BHive
//{
//	class UniformBuffer;
//
//	static constexpr uint32_t sCameraBufferBinding = 0;
//
//	struct CameraBuffer
//	{
//
//	public:
//		struct FCameraData
//		{
//			glm::mat4 Projection{1.0f};
//			glm::mat4 View{1.0f};
//			glm::vec4 NearFar{0.0f, 0.0f, 0.0f, 0.0f};
//			glm::vec4 Position{0.0f, 0.0f, 0.0f, 1.0f};
//			Frustum frustum;
//		};
//
//		void Init();
//
//		void Begin(const glm::mat4 &proj, const glm::mat4 &view);
//
//		const Frustum &GetViewFrustum() const { return mViewFrustum; }
//
//		const FCameraData &GetCameraData() { return mData; }
//
//	private:
//		Ref<UniformBuffer> mBuffer;
//		FCameraData mData;
//		Frustum mViewFrustum;
//	};
// } // namespace BHive