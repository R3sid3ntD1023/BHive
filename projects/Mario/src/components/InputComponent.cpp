#include "InputComponent.h"
#include "objects/GameObject.h"

namespace BHive
{
	void InputComponent::CreateInstance()
	{
		if (Context)
			mContextInstance = Context->CreateInstance();
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

	REFLECT(InputComponent)
	{
		BEGIN_REFLECT(InputComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Context)
			REFLECT_METHOD(ADD_COMPONENT_FUNCTION_NAME, &GameObject::AddComponent<InputComponent>)
				REFLECT_METHOD(REMOVE_COMPONENT_FUNCTION_NAME, &GameObject::RemoveComponent<InputComponent>);
	}
} // namespace BHive