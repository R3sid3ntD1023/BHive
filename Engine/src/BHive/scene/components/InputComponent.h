#pragma once

#include "scene/ActorComponent.h"
#include "input/InputContext.h"

namespace BHive
{

	struct BHIVE InputComponent : public ActorComponent
	{

		TAssetHandle<InputContext> mInputContext;

		virtual void OnBegin() override;

		virtual void OnUpdate(float) override;

		virtual void OnEnd() override;

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		InputContext *GetContext() const { return mContextInstance; }

	private:
		InputContext *mContextInstance = nullptr;

		REFLECTABLEV(ActorComponent)
	};

	REFLECT_EXTERN(InputComponent)

}
