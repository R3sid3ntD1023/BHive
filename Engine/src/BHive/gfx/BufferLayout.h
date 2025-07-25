#pragma once

#include "core/Core.h"
#include "ShaderDataType.h"

namespace BHive
{

	struct BHIVE_API BufferElement
	{
		BufferElement() = default;
		BufferElement(EShaderDataType type, uint32_t divisor = 0, bool normalized = false)
			: Type(type),
			  Divisor(divisor),
			  Normalized(normalized),
			  Size(GetSize()),
			  ComponentCount(GetComponentCount())
		{
		}

		EShaderDataType Type;
		uint32_t Divisor;
		bool Normalized;
		uint32_t Size;
		uint8_t ComponentCount;
		uint64_t Offset;

		uint32_t GetSize();
		uint8_t GetComponentCount();
	};

	struct BHIVE_API BufferLayout
	{
		using Elements = std::vector<BufferElement>;

		BufferLayout() = default;
		BufferLayout(std::initializer_list<BufferElement> elements)
			: mStride(0),
			  mElements(elements)
		{
			CalculateOffsetsAndStride();
		}

		const uint32_t GetStride() const { return mStride; }
		const Elements &GetElements() const { return mElements; }

	private:
		uint32_t mStride = 0;
		Elements mElements;

	private:
		void CalculateOffsetsAndStride();
	};
} // namespace BHive