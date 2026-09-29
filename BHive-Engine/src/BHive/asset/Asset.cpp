#include "Asset.h"

namespace BHive
{
	void Asset::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mHandle);
	}

	void Asset::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mHandle);
	}

	REFLECT(Asset)
	{
		BEGIN_REFLECT(Asset);
	}

} // namespace BHive
