#pragma once

#include "gfx/ResourceSet.h"

namespace BHive
{
	class GLResourceSet : public ResourceSet
	{
	public:
		GLResourceSet(const BindingSetTemplate &setTemplate);

		void SetBuffer(uint32_t binding, BufferPtr buffer) override;

		void SetTexture(uint32_t binding, Ref<Texture> texture, uint32_t mip = 0) override;

		uint32_t GetSetIndex() const override { return mSetIndex; }

		void Update();

	private:
		struct BindingInfo
		{
			EResourceType Type;
			BufferPtr Buffer;
			Ref<Texture> Texture;
			uint32_t MipLevel = 0;
		};

		uint32_t mSetIndex = 0;
		std::unordered_map<uint32_t, BindingInfo> mBindings;
	};
} // namespace BHive