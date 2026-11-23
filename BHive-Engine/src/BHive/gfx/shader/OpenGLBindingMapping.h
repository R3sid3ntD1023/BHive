#pragma once

#include "gfx/Enumerations.h"

namespace BHive
{
	namespace
	{
		constexpr uint32_t LightingStorageBufferBase = 4;
		constexpr uint32_t ShadowStorageBufferBase = 6;
		constexpr uint32_t ShadowTextureBase = 8;
		constexpr uint32_t ImageLightingTextureBase = 12;
	} // namespace

	inline uint32_t GetOpenGLBindingLocation(uint32_t setIndex, uint32_t binding, EResourceType type)
	{
		switch (type)
		{
		case EResourceType::UniformBuffer:
			return setIndex == ERenderingSets::GLOBAL ? binding : binding + setIndex + 1;

		case EResourceType::StorageBuffer:
			switch (setIndex)
			{
			case ERenderingSets::OBJECT:
				return binding;
			case ERenderingSets::LIGHTING:
				return binding + LightingStorageBufferBase;
			case ERenderingSets::SHADOW:
				return binding + ShadowStorageBufferBase;
			default:
				return binding;
			}

		case EResourceType::CombinedImageSampler:
		case EResourceType::SeperatedImage:
		case EResourceType::SeperatedSampler:
		case EResourceType::InputAttachment:
			switch (setIndex)
			{
			case ERenderingSets::SHADOW:
				return binding + ShadowTextureBase;
			case ERenderingSets::IMAGE_BASED_LIGHTING:
				return binding + ImageLightingTextureBase;
			default:
				return binding;
			}

		case EResourceType::StorageImage:
		case EResourceType::Invalid:
		default:
			return binding;
		}
	}
} // namespace BHive
