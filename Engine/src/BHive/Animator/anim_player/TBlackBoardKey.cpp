#include "TBlackBoardKey.h"

namespace BHive
{
	REFLECT(BlackBoardKeyInt)
	{
		BEGIN_REFLECT(BlackBoardKeyInt, "Int Key")
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Value", mValue);
	}

	REFLECT(BlackBoardKeyFloat)
	{
		BEGIN_REFLECT(BlackBoardKeyFloat, "Float Key")
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Value", mValue);
	}

	REFLECT(BlackBoardKeyBool)
	{
		BEGIN_REFLECT(BlackBoardKeyBool, "Bool Key")
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Value", mValue);
	}
} // namespace BHive