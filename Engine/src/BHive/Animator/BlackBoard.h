#pragma once

#include "core/Core.h"
#include "reflection/Reflection.h"
#include "TBlackBoardKey.h"

namespace BHive
{
	class BlackBoard
	{
	public:
		/* data */
		using Keys = std::unordered_map<std::string, Ref<BlackBoardKey>>;

	public:
		BlackBoard() = default;
		~BlackBoard() = default;

		template <typename T>
		void AddKey(const std::string &name)
		{
			mKeys.emplace(name, new TBlackBoardKey<T>());
		}

		void RemoveKey(const std::string &name);

		BlackBoardKey *GetKey(const std::string &name) const;

		const Keys &GetKeys() const { return mKeys; }

		REFLECTABLEV()

	private:
		Keys mKeys;
	};

	REFLECT(BlackBoard)
	{
		BEGIN_REFLECT(BlackBoard)
		REFLECT_PROPERTY_READ_ONLY("Keys", GetKeys);
	}

} // namespace BHive
