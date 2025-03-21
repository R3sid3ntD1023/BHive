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

	void InputComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(TAssetHandle<InputContext>(Context));
	}

	void InputComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(TAssetHandle<InputContext>(Context));
	}

	REFLECT(InputComponent)
	{
		BEGIN_REFLECT(InputComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Context) COMPONENT_IMPL();
	}
} // namespace BHive