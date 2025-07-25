#pragma once

#include "core/Core.h"
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

		bool AddKey(const std::string &name, const Ref<BlackBoardKey> &key);

		void RemoveKey(const std::string &name);

		Ref<BlackBoardKey> GetKey(const std::string &name) const;

		const Keys &GetKeys() const { return mKeys; }

		Keys &GetKeys() { return mKeys; }

		void Save(cereal::BinaryOutputArchive &ar) const;
		void Load(cereal::BinaryInputArchive &ar);

		REFLECTABLEV()

	private:
		Keys mKeys;
	};

	REFLECT_EXTERN(BlackBoard)

} // namespace BHive
