#include "AssociativeContainerModifiers.h"

namespace BHive
{
	bool FAssociativeViewClear::modify(rttr::variant_associative_view &view)
	{
		view.clear();
		return true;
	}

	bool FAssociativeViewDelete::modify(rttr::variant_associative_view &view)
	{
		return view.erase(mKey);
	}

	bool FAssociativeViewChange::modify(rttr::variant_associative_view &view)
	{
		view.erase(mKey);
		return view.insert(mNewKey, mValue).second;
	}

	bool FAssociativeViewAppend::modify(rttr::variant_associative_view &view)
	{
		auto key = mKey.create();
		auto value = mValue.create();

		return view.insert(key, value).second;
	}

	bool FAssociativeViewRemove::modify(rttr::variant_associative_view &view)
	{
		if (!view.get_size())
			return false;

		auto key = view.end() - 1;
		return view.erase(key.get_key().extract_wrapped_value());
	}
} // namespace BHive