#pragma once

#include "core/Core.h"

namespace BHive
{
	DECLARE_ENUM(BitFlags)
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
} // namespace BHive