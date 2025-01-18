#pragma once

#include "reflection/Reflection.h"
#include "serialization/Serialization.h"

namespace BHive
{
	class AnimGraph;
	class AnimationNodeBase;

	class AnimGraphNodeBase
	{
	public:
		AnimGraphNodeBase(AnimGraph *graph)
			: mOwningGraph(graph)
		{
		}

		virtual ~AnimGraphNodeBase() = default;

		virtual void CollectDesendents(std::vector<Ref<AnimGraphNodeBase>> &children) const {};

		virtual void Serialize(cereal::BinaryOutputArchive &ar) const { ar(mID); };

		virtual void Serialize(cereal::BinaryInputArchive &ar) { ar(mID); };

		virtual Ref<AnimationNodeBase> Clone() = 0;

		const UUID &GetID() const { return mID; }

		REFLECTABLEV()

	private:
		AnimGraph *mOwningGraph{nullptr};
		UUID mID;
	};

	REFLECT(AnimGraphNodeBase)
	{
		BEGIN_REFLECT(AnimGraphNodeBase);
	}

} // namespace BHive