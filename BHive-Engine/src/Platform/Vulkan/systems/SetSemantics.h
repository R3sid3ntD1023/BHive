#pragma once

namespace BHive
{
	enum class ESetSemantic
	{
		None, 
		Global, //set 0
		Material,//set 1
		Object //set 2
	};

	inline ESetSemantic GetSetSemantic(uint32_t setIndex)
	{
		switch (setIndex)
		{
		case 0: 
			return ESetSemantic::Global;
		case 1:
			return ESetSemantic::Material;
		case 2:
			return ESetSemantic::Object;
		default:
			return ESetSemantic::None;
		}
	}
}