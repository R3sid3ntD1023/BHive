#pragma once

#include "Component.h"

namespace BHive
{
	class AudioSource;

	struct AudioComponent : public Component
	{
	public:

		void Begin() override;

		void End() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		virtual void Save(cereal::JSONOutputArchive &ar) const;
		virtual void Load(cereal::JSONInputArchive &ar);

		bool AutoPlay{false};

		Ref<AudioSource> Audio;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(AudioComponent)
}