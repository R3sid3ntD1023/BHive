#include "InputComponent.h"
#include "asset/AssetManager.h"

namespace BHive
{
	void InputComponent::OnBegin()
	{
		if (mInputContext)
			mContextInstance = mInputContext->CreateInstance();
	}

	void InputComponent::OnUpdate(float)
	{
		if (mContextInstance)
			mContextInstance->process();
	}

	void InputComponent::OnEnd()
	{
		if (mContextInstance)
			delete mContextInstance;
	}
	void InputComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		Component::Save(ar);
		ar(MAKE_NVP("InputContext", mInputContext));
	}
	void InputComponent::Load(cereal::JSONInputArchive &ar)
	{
		Component::Load(ar);
		ar(MAKE_NVP("InputContext", mInputContext));
	}

	REFLECT(InputComponent)
	{
		BEGIN_REFLECT(InputComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("InputContext", mInputContext)
				REFLECT_PROPERTY_READ_ONLY("InputContextRef", mContextInstance);
	}
} // namespace BHive