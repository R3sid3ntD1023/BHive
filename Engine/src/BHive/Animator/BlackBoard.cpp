#include "BlackBoard.h"

namespace BHive
{
	void BlackBoard::RemoveKey(const std::string &name)
	{
		ASSERT(mKeys.contains(name));

		mKeys.erase(name);
	}

	bool BlackBoard::AddKey(const std::string &name, const Ref<BlackBoardKey> &key)
	{
		if (mKeys.contains(name))
			return false;

		mKeys.emplace(name, key);
		return true;
	}

	Ref<BlackBoardKey> BlackBoard::GetKey(const std::string &name) const
	{
		if (mKeys.contains(name))
			return mKeys.at(name);

		return nullptr;
	}

	void BlackBoard::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mKeys.size());

		for (auto &[name, key] : mKeys)
		{
			ar(name, key->get_type());
		}
	}

	void BlackBoard::Load(cereal::BinaryInputArchive &ar)
	{
		size_t size = 0;
		ar(size);

		for (size_t i = 0; i < size; i++)
		{
			std::string name;
			rttr::type key_type = InvalidType;

			ar(name, key_type);

			if (auto key = key_type.create().get_value<Ref<BlackBoardKey>>())
			{
				AddKey(name, key);
			}
		}
	}

} // namespace BHive
