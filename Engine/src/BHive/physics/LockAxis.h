#pragma once

#include "core/Core.h"

namespace BHive
{
	DECLARE_ENUM(BitFlags)
	enum ELockAxis : int
	{
		NoAxis = 0,
		AxisX = BIT(0),
		AxisY = BIT(1),
		AxisZ = BIT(2),
		AxisXY = AxisX | AxisY,
		AxisXZ = AxisX | AxisZ,
		AxisYZ = AxisY | AxisZ,
		AxisXYZ = AxisX | AxisY | AxisZ
	};
} // namespace BHive
