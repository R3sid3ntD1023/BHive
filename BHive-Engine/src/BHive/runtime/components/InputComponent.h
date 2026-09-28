#pragma once

#include "input/InputContext.h"
#include "runtime/Component.h"

namespace BHive
{
	struct BHIVE_API InputComponent : public Component, public ITickable
	{
		AssetHandle<InputContext> Context;

		InputComponent() = default;

		InputComponent(const InputComponent &) = default;

		void CreateInstance();

		InputContext *GetInstance() const { return mContextInstance; }

		void DestroyInstance();

		void Begin() override;

		void Update(float) override;

		void End() override;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component, ITickable)

	private:
		InputContext *mContextInstance = nullptr;
	};

	REFLECT_EXTERN(InputComponent)

} // namespace BHive
