#pragma once

#include "core/Core.h"
#include "reflection/Reflection.h"

namespace BHive
{
	struct SubClassOf
	{
	};

	template <typename T>
	struct TSubClassOf : public SubClassOf
	{
		using BaseType = T;

		TSubClassOf() = default;
		TSubClassOf(const TSubClassOf &other) = default;

		TSubClassOf(const rttr::type &type)
			: mType(type)
		{
		}

		template <typename U>
		TSubClassOf(const TSubClassOf<U> &other)
			requires(std::is_base_of_v<T, U>)
			: mType(other.mType)
		{
		}

		template <typename U>
		TSubClassOf &operator=(const TSubClassOf<U> &rhs)
			requires(std::is_base_of_v<T, U>)
		{
			mType = rhs.mType;
			return *this;
		}

		TSubClassOf &operator=(const rttr::type &rhs)
		{
			ASSERT(rhs.is_derived_from<T>(), "Type is not derived from T");

			mType = rhs;
			return *this;
		}

		operator bool() const { return mType != InvalidType; }

		const rttr::type &get() const { return mType; }

	private:
		rttr::type mType = InvalidType;

		template <typename U>
		friend struct TSubClassOf;
	};

} // namespace BHive

namespace rttr
{
	template <typename T>
	struct wrapper_mapper<::BHive::TSubClassOf<T>>
	{
		using wrapped_type = rttr::type;
		using type = ::BHive::TSubClassOf<T>;

		static wrapped_type get(const type &obj) { return obj.get(); }
	};
} // namespace rttr