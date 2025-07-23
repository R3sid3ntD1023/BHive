#pragma once

#include "core/Core.h"
#include "reflection/Reflection.h"

namespace BHive
{
	struct BHIVE_API SubClassOf{REFLECTABLEV()};

	template <typename T>
	struct BHIVE_API TSubClassOf : public SubClassOf
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

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mType);
		}

		REFLECTABLEV(SubClassOf)

	private:
		rttr::type mType = InvalidType;

		static inline int reflect()
		{
			auto class_name = std::format("TSubclassOf<{}>", rttr::type::get<T>().get_name().data());
			rttr::registration::class_<TSubClassOf<T>>(class_name)
				.constructor<const rttr::type &>()(rttr::policy::ctor::as_object);
			return 0;
		}

		static inline int auto_register = reflect();

		template <typename U>
		friend struct TSubClassOf;
	};

	REFLECT(SubClassOf)
	{
		BEGIN_REFLECT(SubClassOf);
	}

} // namespace BHive

namespace rttr
{
	template <typename T>
	struct wrapper_mapper<BHive::TSubClassOf<T>>
	{
		using wrapped_type = rttr::type;
		using type = BHive::TSubClassOf<T>;

		inline static wrapped_type get(const type &obj) { return obj.get(); }

		inline static type create(const wrapped_type &value) { return type(value); }
	};
} // namespace rttr