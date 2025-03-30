#pragma once

#include "core/reflection/Reflection.h"

namespace BHive
{
	REFLECT_ENUM(BHive::EPropertyMetaData_Flags = BHive::EPropertyFlags_BitFlags)
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
