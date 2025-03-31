#pragma once

#include "reflection/Reflection.h"

namespace BHive
{
	DECLARE_STRUCT()
	struct TEnumAsByteBase
	{
		virtual void Set(const int &value) = 0;

		virtual int Get() const = 0;

		virtual rttr::enumeration GetEnumeration() const = 0;

		REFLECTABLE_CLASS()
	};

	template <typename TEnum>
	struct TEnumAsByte : TEnumAsByteBase
	{
		using underlying_type = typename std::underlying_type_t<TEnum>;

		TEnumAsByte() = default;

		explicit TEnumAsByte(underlying_type value)
			: mValue((TEnum)value)
		{
		}

		TEnumAsByte(const TEnum &enum_value)
			: mValue(enum_value)
		{
		}

		virtual void Set(const int &value) { mValue = (TEnum)(underlying_type)value; }

		virtual int Get() const { return (int)(underlying_type)(TEnum)mValue; }

		virtual rttr::enumeration GetEnumeration() const { return rttr::type::get<TEnum>().get_enumeration(); };

		bool HasFlag(const TEnum &rhs) const { return (mValue | rhs) != 0; }

		TEnumAsByte &operator=(const int &rhs)
		{
			mValue = (TEnum)(underlying_type)rhs;
			return *this;
		}

		TEnumAsByte &operator=(const TEnum &rhs)
		{
			mValue = rhs;
			return *this;
		}

		bool operator==(const TEnum &rhs) const { return mValue == rhs; }

		bool operator==(const TEnumAsByte &rhs) const { return mValue == rhs.mValue; }

		operator TEnum() const { return mValue; }

		TEnumAsByte operator~() const { return ~mValue; }

		TEnumAsByte operator&(const TEnum &rhs) const { return (TEnum)((underlying_type)mValue & (underlying_type)rhs); }

		TEnumAsByte operator|(const TEnum &rhs) const { return (TEnum)((underlying_type)mValue | (underlying_type)rhs); }

		TEnumAsByte &operator&=(const TEnum &rhs) { return *this = *this & rhs; }

		TEnumAsByte &operator|=(const TEnum &rhs) { return *this = *this | rhs; }

		template <typename A>
		inline int SaveMinimal(const A &ar) const
		{
			return Get();
		}

		template <typename A>
		inline void LoadMinimal(const A &ar, const int &value)
		{
			mValue = (TEnum)value;
		}

		REFLECTABLE_CLASS(TEnumAsByteBase)

	private:
		TEnum mValue;
	};

} // namespace BHive
