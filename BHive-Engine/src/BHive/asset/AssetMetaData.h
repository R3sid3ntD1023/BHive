#pragma once

#include "core/Core.h"
#include "core/UUID.h"

namespace BHive
{

	struct BHIVE_API FAssetMetaData
	{
		UUID Handle;

		rttr::type Type = InvalidType;

		std::string Name = "";

		std::filesystem::path SourcePath = "";

		uint64_t SourceTimeStamp = 0;

		operator bool() const { return Type != InvalidType; }

		template <typename A>
		inline void Serialize(A &ar)
		{
			ar(MAKE_NVP(Handle), MAKE_NVP(Type), MAKE_NVP(Name), MAKE_NVP(SourcePath), MAKE_NVP(SourceTimeStamp));
		}

		REFLECTABLE()
	};

	REFLECT_INLINE(FAssetMetaData)
	{
		BEGIN_REFLECT(FAssetMetaData)
		REFLECT_PROPERTY("Handle", Handle)
		REFLECT_PROPERTY("Type", Type)
		REFLECT_PROPERTY("SourcePath", SourcePath)
		REFLECT_PROPERTY("SourceTimeStamp", SourceTimeStamp)
		REFLECT_PROPERTY("Name", Name);
	}
} // namespace BHive