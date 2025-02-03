#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
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

	REFLECT(ELockAxis)
	{
		BEGIN_REFLECT_ENUM(ELockAxis)
		(ENUM_VALUE(NoAxis), ENUM_VALUE(AxisX), ENUM_VALUE(AxisY), ENUM_VALUE(AxisZ));
	}
} // namespace BHive
