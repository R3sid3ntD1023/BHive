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

	void Asset::Save(cereal::JSONOutputArchive &ar) const
	{
		ar(MAKE_NVP("Handle", mHandle), MAKE_NVP("Name", mName));
	}

	void Asset::Load(cereal::JSONInputArchive &ar)
	{
		ar(MAKE_NVP("Handle", mHandle), MAKE_NVP("Name", mName));
	}

	REFLECT(Asset)
	{
		BEGIN_REFLECT(Asset);
	}

} // namespace BHive
