#include "AnimGraphNodeBase.h"

namespace BHive
{
	void AnimGraphNodeBase::Serialize(cereal::BinaryOutputArchive &ar)
	{
		auto pos = getPos();
		ar(mID, glm::vec2(pos.x, pos.y));
	}

	void AnimGraphNodeBase::Serialize(cereal::BinaryInputArchive &ar)
	{
		glm::vec2 pos;
		ar(mID, pos);

		setPos({pos.x, pos.y});
	}

	REFLECT(AnimGraphNodeBase)
	{
		BEGIN_REFLECT(AnimGraphNodeBase);
	}

} // namespace BHive