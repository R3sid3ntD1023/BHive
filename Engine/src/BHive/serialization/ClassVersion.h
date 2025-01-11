#pragma once

namespace BHive
{
	template<typename T>
	struct class_version
	{
		static constexpr unsigned version = 0;
	};
}

#define REGISTER_CLASS_VERSION(cls, v)			\
	template<>\
	::BHive::class_version<cls>					\
	{											\
		static constexpr unsigned version =  v;   \
	}