#pragma once

#include <rttr/type.h>

namespace rttr
{

	template <typename TArchive>
	void Serialize(const TArchive &ar, const type &obj)
	{
		std::string type_name = obj.get_name().data();
		ar(type_name);
	}

	template <typename TArchive>
	void Deserialize(const TArchive &ar, type &obj)
	{
		std::string type_name;
		ar(type_name);
		obj = type::get_by_name(type_name);
	}
}