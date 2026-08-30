#pragma once

#include "core/Core.h"
#include "gfx/material/MaterialTable.h"
#include "core/math/Transform.h"
#include "core/math/boundingbox/AABB.h"
#include "gfx/mesh/MeshData.h"
#include "core/delegates/MultiEventDelegate.h"
#include "RenderData.h"

namespace BHive
{
	DECLARE_MULTI_EVENT(FRenderQueueChanged)

	struct FRenderQueue
	{
		std::vector<FMeshSubmissionContext> Contexts;
		SubMeshSubmissions Opaque;
		SubMeshSubmissions Transparent;
		SubMeshSubmissions ShadowPassRenderData;
		SubMeshSubmissions RenderPassRenderData;

		void Init(uint32_t maxObjects);

		FMeshSubmissionContext &ResolveContext(ContextHandle h);

		uint32_t AddSubmissionContext();

		void RemoveSubmissionsForContext(ContextHandle h);

		void AddSubmissionsForMesh(ContextHandle h, MeshPtr mesh);

		ContextHandle AddMesh(const FMeshSubmissionRequest &request);

		void AddSubmission(FSubMeshSubmission &submission);

		void SortTransparent(const glm::mat4 &view);

		void BuildQueue(const glm::mat4 &view);

		void Reset();

		bool IsHandleValid(ContextHandle handle) const;

		FRenderQueueChangedEvent OnQueueChanged;

	private:
		uint32_t mGenerationVersion = 1;
	};
} // namespace BHive