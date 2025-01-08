#pragma once

#include <stdint.h>
#include "reflection/Reflection.h"

namespace BHive
{
	enum ECollisionChannel : uint16_t
	{
		CollisionChannel_None = 0,
		CollisionChannel_0 = BIT(1),
		CollisionChannel_1 = BIT(2),
		CollisionChannel_2 = BIT(3),
		CollisionChannel_3 = BIT(4),
		CollisionChannel_4 = BIT(5),
		CollisionChannel_5 = BIT(6),
		CollisionChannel_6 = BIT(7),
		CollisionChannel_7 = BIT(8),
		CollisionChannel_8 = BIT(9),
		CollisionChannel_9 = BIT(10),
		CollisionChannel_10 = BIT(11),
		CollisionChannel_11 = BIT(12),
		CollisionChannel_12 = BIT(13),
		CollisionChannel_13 = BIT(14),
		CollisionChannel_14 = BIT(15),
		CollisionChannel_All = 65535
	};

	REFLECT(ECollisionChannel)
	{
		BEGIN_REFLECT_ENUM(ECollisionChannel)
		(ENUM_VALUE(CollisionChannel_None), ENUM_VALUE(CollisionChannel_0), 
			ENUM_VALUE(CollisionChannel_1), ENUM_VALUE(CollisionChannel_2), 
			ENUM_VALUE(CollisionChannel_3), ENUM_VALUE(CollisionChannel_4),
			ENUM_VALUE(CollisionChannel_5), ENUM_VALUE(CollisionChannel_6), 
			ENUM_VALUE(CollisionChannel_7), ENUM_VALUE(CollisionChannel_8), 
			ENUM_VALUE(CollisionChannel_9), ENUM_VALUE(CollisionChannel_10), 
			ENUM_VALUE(CollisionChannel_11),ENUM_VALUE(CollisionChannel_12), 
			ENUM_VALUE(CollisionChannel_13),ENUM_VALUE(CollisionChannel_14), 
			ENUM_VALUE(CollisionChannel_All));
	}
}