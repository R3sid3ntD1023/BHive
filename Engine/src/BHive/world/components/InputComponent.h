#pragma once

#include "World/Component.h"
#include "input/InputContext.h"

namespace BHive
{

	struct BHIVE_API InputComponent : public Component, public ITickable
	{

		InputComponent() = default;
		InputComponent(const InputComponent &) = default;

		Ref<InputContext> Context;

		void CreateInstance();
		InputContext *GetInstance() const { return mContextInstance; }
		void DestroyInstance();

		void Begin() override;
		void Update(float) override;
		void End() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component, ITickable)

	private:
		InputContext *mContextInstance = nullptr;
	};

	REFLECT_EXTERN(InputComponent)

} // namespace BHive
