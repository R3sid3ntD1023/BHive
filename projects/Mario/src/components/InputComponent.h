#pragma once

#include "Component.h"
#include "input/InputContext.h"

namespace BHive
{
	REFLECT_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct InputComponent : public Component
	{
		REFLECT_CONSTRUCTOR()
		InputComponent() = default;
		InputComponent(const InputComponent &) = default;

		REFLECT_PROPERTY()
		Ref<InputContext> Context;

		void CreateInstance();
		InputContext *GetInstance() const { return mContextInstance; }
		void DestroyInstance();

		void Begin() override;
		void Update(float) override;
		void End() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)

	private:
		InputContext *mContextInstance = nullptr;
	};

} // namespace BHive
