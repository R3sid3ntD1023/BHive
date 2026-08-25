#include "RenderQueue.h"
#include "gfx/mesh/BaseMesh.h"

namespace BHive
{
	void FRenderQueue::Init(uint32_t maxObjects)
	{
		Contexts.reserve(maxObjects);
	}

	FMeshSubmissionContext &FRenderQueue::ResolveContext(ContextHandle h)
	{
		auto &ctx = Contexts.at(h.ContextIndex);

		if (!ctx.Active || ctx.Generation != h.Generation)
			throw std::runtime_error("Stale  or invalid context handle");

		return ctx;
	}

	uint32_t FRenderQueue::AddSubmissionContext()
	{
		for (uint32_t i = 0; i < Contexts.size(); i++)
		{
			if (!Contexts[i].Active)
			{
				Contexts[i].Active = true;
				return i;
			}
		}

		Contexts.emplace_back();
		return Contexts.size() - 1;
	}

	void FRenderQueue::RemoveSubmissionsForContext(ContextHandle h)
	{
		const uint32_t ctxIndex = h.ContextIndex;

		auto pred = [ctxIndex](const FSubMeshSubmission &s) { return s.Context.ContextIndex == ctxIndex; };

		Opaque.erase(std::remove_if(Opaque.begin(), Opaque.end(), pred), Opaque.end());
		Transparent.erase(std::remove_if(Transparent.begin(), Transparent.end(), pred), Transparent.end());

		auto &ctx = Contexts[ctxIndex];
		ctx.Active = false;
	}

	void FRenderQueue::AddSubmissionsForMesh(ContextHandle h, Ref<BaseMesh> mesh)
	{
		const uint32_t ctxIndex = h.ContextIndex;

		auto &ctx = Contexts.at(ctxIndex);
		ctx.Active = true;
		ctx.VAO = mesh->GetVertexArray();
		const auto &materials = mesh->GetMaterialTable();
		const auto &subMeshes = mesh->GetSubMeshes();

		for (auto &s : subMeshes)
		{
			auto material = materials.get_material(s.MaterialIndex);
			if (!material)
				continue;

			FSubMeshSubmission sub{};
			sub.Context = {ctxIndex, ctx.Generation};
			sub.SubMesh = s;
			sub.BoundingBox = mesh->GetBoundingBox();
			sub.BitFlags[0] = material->IsTransparent();
			sub.BitFlags[1] = material->ShouldCastShadows();
			AddSubmission(sub);
		}
	}

	ContextHandle FRenderQueue::AddMesh(const FMeshSubmissionRequest &request)
	{

		auto ctxIndex = AddSubmissionContext();
		auto &ctx = Contexts.at(ctxIndex);
		ctx.Active = true;
		ctx.Transform = request.Transform;
		ctx.EntityID = request.EntityID;
		ctx.InstanceTransforms = request.InstanceTransforms;
		ctx.BoneTransforms = request.BoneTransforms;
		ctx.VAO = request.Mesh->GetVertexArray();
		ctx.Generation = ++mGenerationVersion;
		ctx.Materials = request.Materials;

		const auto &subMeshes = request.Mesh->GetSubMeshes();
		for (auto &s : subMeshes)
		{
			auto material = request.Materials.get_material(s.MaterialIndex);
			if (!material)
				continue;

			FSubMeshSubmission sub{};
			sub.Context = {ctxIndex, mGenerationVersion};
			sub.SubMesh = s;
			sub.BoundingBox = request.Mesh->GetBoundingBox();
			sub.BitFlags[0] = material->IsTransparent();
			sub.BitFlags[1] = material->ShouldCastShadows();
			AddSubmission(sub);
		}

		return ContextHandle{ctxIndex, ctx.Generation};
	}

	void FRenderQueue::AddSubmission(FSubMeshSubmission &submission)
	{
		auto flags = submission.BitFlags;
		auto &submissions = flags[0] ? Transparent : Opaque;
		submissions.emplace_back(submission);

		if (flags[1])
			ShadowPassRenderData.emplace_back(submission);
	}

	void FRenderQueue::SortTransparent(const glm::mat4 &view)
	{
		// back-to-front
		static auto sorting = [=](FSubMeshSubmission &a, FSubMeshSubmission &b)
		{
			auto &aCtx = ResolveContext(a.Context);
			auto &bCtx = ResolveContext(b.Context);

			float za = (view * glm::vec4(aCtx.Transform.GetTranslation(), 1.0f)).z;
			float zb = (view * glm::vec4(bCtx.Transform.GetTranslation(), 1.0f)).z;
			return za > zb;
		};

		std::sort(Transparent.begin(), Transparent.end(), sorting);
	}

	void FRenderQueue::BuildQueue(const glm::mat4 &view)
	{
		SortTransparent(view);
	}

	void FRenderQueue::Reset()
	{
		Contexts.clear();
		Opaque.clear();
		Transparent.clear();
		ShadowPassRenderData.clear();
		RenderPassRenderData.clear();
		mGenerationVersion = 0;
	}

	bool FRenderQueue::IsHandleValid(ContextHandle handle) const
	{
		if (handle.ContextIndex >= Contexts.size())
			return false;

		const auto &ctx = Contexts.at(handle.ContextIndex);
		if (!ctx.Active)
			return false;

		if (ctx.Generation != handle.Generation)
			return false;

		return true;
	}
} // namespace BHive