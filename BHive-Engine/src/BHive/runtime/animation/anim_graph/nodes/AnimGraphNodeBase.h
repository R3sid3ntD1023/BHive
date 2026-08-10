#pragma once

#include "core/Core.h"
#include "core/UUID.h"
#pragma warning(push, 0)
#include <ImNodeFlow.h>
#pragma warning(pop)

namespace BHive
{
	class AnimGraph;
	class AnimationNodeBase;

	class BHIVE_API AnimGraphNodeBase : public ImFlow::BaseNode
	{
	public:
		AnimGraphNodeBase() = default;

		virtual ~AnimGraphNodeBase() = default;

		virtual void CollectDesendents(std::vector<Ref<AnimGraphNodeBase>> &children) const {};

		virtual void Serialize(cereal::BinaryOutputArchive &ar);

		virtual void Serialize(cereal::BinaryInputArchive &ar);

		virtual Ref<AnimationNodeBase> Clone() { return nullptr; }

		const UUID &GetID() const { return mID; }

		AnimGraph *GetGraph() const { return mParentGraph; }

		REFLECTABLEV()

	private:
		UUID mID;

		AnimGraph *mParentGraph = nullptr;

		friend class AnimGraph;
	};

	REFLECT_EXTERN(AnimGraphNodeBase)
} // namespace BHive