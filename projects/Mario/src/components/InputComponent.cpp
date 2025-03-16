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

	void InputComponent::Begin()
	{
		CreateInstance();
	}

	void InputComponent::Update(float)
	{
		if (mContextInstance)
			mContextInstance->process();
	}

	void InputComponent::End()
	{
		DestroyInstance();
	}
} // namespace BHive