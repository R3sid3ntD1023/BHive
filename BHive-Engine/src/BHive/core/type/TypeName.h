#pragma once

namespace BHive
{
	template <typename T>
	struct TypeName;

#define REGISTER_TYPE(T)                         \
	template <>                                  \
	struct TypeName<T>                           \
	{                                            \
		static constexpr const char *value = #T; \
	};
} // namespace BHive
