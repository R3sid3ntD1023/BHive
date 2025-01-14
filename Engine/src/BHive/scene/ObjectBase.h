#pragma once

#include "core/Core.h"
#include "core/UUID.h"
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"

namespace BHive
{
	struct FObjectInitializer
	{
		std::string mName = "NewObject";
	};

	class ObjectBase
	{
	private:
		/* data */
	public:
		ObjectBase(const FObjectInitializer &initializer = {});
		ObjectBase(const ObjectBase &other) = default;

		virtual ~ObjectBase() = default;

		const std::string &GetName() const { return mData.mName; }
		void SetName(const std::string &name);

		void GenerateNewUUID();

		const UUID &GetUUID() const { return mID; }

		virtual ObjectBase *Copy();

		virtual void Save(cereal::JSONOutputArchive &ar) const;

		virtual void Load(cereal::JSONInputArchive &ar);

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		bool operator==(const ObjectBase &rhs) const { return mID == rhs.mID; }
		bool operator!=(const ObjectBase &rhs) const { return mID != rhs.mID; }

	private:
		FObjectInitializer mData;
		UUID mID;

		REFLECTABLEV()
	};

	REFLECT_EXTERN(ObjectBase)
} // namespace BHive
