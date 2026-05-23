#pragma once

#include "ISetManager.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	enum class EBindingUpdateRate
	{
		Static,
		PerFrame
	};

	struct FBindingInfo
	{
		uint32_t Binding = 0;
		EResourceType Type{};
		EResourceCategory Category{};
		EBindingUpdateRate UpdateRate{};

		Ref<BufferBase> Buffer;
		Ref<Texture> Texture;
		uint32_t MipLevel = 0;
	};

	class SetManagerBase : public ISetManager
	{
	public:
		SetManagerBase(uint32_t setIndex)
			: mSetIndex(setIndex)
		{}

		virtual ~SetManagerBase() = default;

		const std::vector<FBindingInfo> &GetBindings() const { return mBindings; }

		uint32_t GetSetIndex() const { return mSetIndex; }

	protected:
		uint32_t mSetIndex;
		std::vector<FBindingInfo> mBindings;

	public:

		void BuildBindings(const FShaderReflectionLookUp &refl)
		{
			auto &setBindings = refl.GetSetBindings(mSetIndex);
			mBindings.reserve(setBindings.size());

			for (auto &r : setBindings)
			{
				FBindingInfo info{};
				info.Binding = r.binding;
				info.Type = r.kind;
				info.Category = GetCategory(r.kind);
				info.UpdateRate = InferUpdateRate(r.kind, mSetIndex);
				mBindings.push_back(info);
			}
		}

		static EBindingUpdateRate InferUpdateRate(EResourceType type, uint32_t setIndex)
		{
			if (setIndex == 0)
				return EBindingUpdateRate::PerFrame;

			switch (type)
			{
			case EResourceType::UniformBuffer:
			case EResourceType::StorageBuffer:
			case EResourceType::StorageImage:
			case EResourceType::InputAttachment:
				return EBindingUpdateRate::PerFrame;
			case EResourceType::CombinedImageSampler:
			case EResourceType::SeperatedImage:
			case EResourceType::SeperatedSampler:
				return EBindingUpdateRate::Static;
			default:
				return EBindingUpdateRate::PerFrame;
			}
		}
	};
}