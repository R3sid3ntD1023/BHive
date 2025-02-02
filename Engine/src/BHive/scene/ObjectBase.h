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

	DECLARE_EVENT(FOnDestroyed, class ObjectBase *);

	class ObjectBase
	{
	private:
		/* data */
	public:
		ObjectBase(const FObjectInitializer &initializer = {});
		ObjectBase(const ObjectBase &other) = default;

		virtual ~ObjectBase() = default;

		void Destroy(bool descendents = false);

		virtual void OnDestroyed(bool descendents) {};

		void SetName(const std::string &name);

		void GenerateNewUUID();

		const std::string &GetName() const { return mData.mName; }

		const UUID &GetUUID() const { return mID; }

		virtual ObjectBase *Copy();

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		bool operator==(const ObjectBase &rhs) const { return mID == rhs.mID; }
		bool operator!=(const ObjectBase &rhs) const { return mID != rhs.mID; }

	public:
		FOnDestroyedEvent OnDestroyedEvent;

	private:
		FObjectInitializer mData;
		UUID mID;

		REFLECTABLEV()
	};

	REFLECT_EXTERN(ObjectBase)
} // namespace BHive
