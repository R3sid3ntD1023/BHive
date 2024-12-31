#pragma once

#include "core/Core.h"

namespace BHive
{
	enum class EShaderDataType { Float, Float2, Float3, Float4, Int , Int2, Int3, Int4, Bool , Mat3, Mat4};

	struct BHIVE BufferElement
	{
		BufferElement() = default;
		BufferElement(EShaderDataType type, uint32_t divisor = 0, bool normalized = false)
			:Type(type), Divisor(divisor), Normalized(normalized), Size(GetSize()), ComponentCount(GetComponentCount())
		{}

		EShaderDataType Type;
		uint32_t Divisor;
		bool Normalized;
		uint32_t Size;
		uint8_t ComponentCount;
		uint64_t Offset;

		uint32_t GetSize();
		uint8_t GetComponentCount();
	};

	struct BHIVE BufferLayout
	{
		using Elements = std::vector<BufferElement>;

		BufferLayout() = default;
		BufferLayout(std::initializer_list<BufferElement> elements)
			:mStride(0), mElements(elements)
		{
			CalculateOffsetsAndStride();
		}

		const uint32_t GetStride() const { return mStride; }
		const Elements& GetElements() const { return mElements; }

	private:
		uint32_t mStride;
		Elements mElements;

	private:
		void CalculateOffsetsAndStride();
	};
}