#pragma once

#include <rttr/variant.h>
#include <rttr/variant_associative_view.h>

namespace BHive
{
	struct FAssociativeViewModifier
	{
		virtual bool modify(rttr::variant_associative_view &view) = 0;
	};

	struct FAssociativeViewClear : public FAssociativeViewModifier
	{
		bool modify(rttr::variant_associative_view &view);
	};

	struct FAssociativeViewDelete : public FAssociativeViewModifier
	{
		FAssociativeViewDelete(const rttr::variant &key)
			: mKey(key)
		{
		}

		bool modify(rttr::variant_associative_view &view);

	private:
		rttr::variant mKey;
	};

	struct FAssociativeViewChange : public FAssociativeViewModifier
	{
		FAssociativeViewChange(const rttr::variant &key, const rttr::variant &new_key, const rttr::variant &value)
			: mKey(key),
			  mNewKey(new_key),
			  mValue(value)
		{
		}

		bool modify(rttr::variant_associative_view &view);

	private:
		rttr::variant mKey;
		rttr::variant mNewKey;
		rttr::variant mValue;
	};

	struct FAssociativeViewAppend : public FAssociativeViewModifier
	{
		FAssociativeViewAppend(const rttr::type &key, const rttr::type &value)
			: mKey(key),
			  mValue(value)
		{
		}

		bool modify(rttr::variant_associative_view &view);

	private:
		rttr::type mKey;
		rttr::type mValue;
	};

	struct FAssociativeViewRemove : public FAssociativeViewModifier
	{
		bool modify(rttr::variant_associative_view &view);
	};
} // namespace BHive