#include "InputComponent.h"

namespace BHive
{
	void InputComponent::CreateInstance()
	{
		if (mInputContext)
			mContextInstance = mInputContext->CreateInstance();
	}

	void InputComponent::DestroyInstance()
	{
		mContextInstance = nullptr;
	}
} // namespace BHive