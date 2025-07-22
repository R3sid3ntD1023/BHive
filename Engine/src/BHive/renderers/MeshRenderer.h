#pragma once

#include "core/Core.h"
#include "core/math/Transform.h"

namespace BHive
{
	class StaticMesh;
	class SkeletalMesh;
	class SkeletalPose;
	class BaseMesh;

	class MeshRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin();
		static void End();

		static void DrawMesh(
			const Ref<StaticMesh> &mesh, const glm::mat4 &transform = {1.0f}, const glm::mat4 *instances = nullptr,
			size_t instanceCount = 0);

		static void DrawMesh(
			const Ref<SkeletalMesh> &mesh, const SkeletalPose &pose, const glm::mat4 &transform = {1.0f},
			const glm::mat4 *instances = nullptr, size_t instanceCount = 0);

	private:
		static bool IsMeshCulled(const Ref<BaseMesh> &mesh, const glm::mat4 &transform);
	};
} // namespace BHive
