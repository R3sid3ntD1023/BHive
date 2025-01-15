#include "Asset.h"

namespace BHive
{
	void Asset::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mHandle, mName);
	}

	void Asset::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mHandle, mName);
	}

} // namespace BHive
