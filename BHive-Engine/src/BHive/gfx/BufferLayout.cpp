#include "BufferLayout.h"

namespace BHive
{
    uint32_t BufferElement::GetSize()
    {
        switch (Type)
        {
        case BHive::EShaderDataType::Float:     return 4;
        case BHive::EShaderDataType::Float2:    return 4 * 2;
        case BHive::EShaderDataType::Float3:    return 4 * 3;
        case BHive::EShaderDataType::Float4:    return 4 * 4;
        case BHive::EShaderDataType::Int:       return 4 * 1;
        case BHive::EShaderDataType::Int2:      return 4 * 2;
        case BHive::EShaderDataType::Int3:      return 4 * 3;
        case BHive::EShaderDataType::Int4:      return 4 * 4;
        case BHive::EShaderDataType::Bool:      return 1;
        case BHive::EShaderDataType::Mat3:      return 4 * 3 * 3;
        case BHive::EShaderDataType::Mat4:      return 4 * 4 * 4;
        default:
            break;
        }
        return 0;
    }

    uint8_t BufferElement::GetComponentCount()
    {
        switch (Type)
        {
        case BHive::EShaderDataType::Float:     return 1;
        case BHive::EShaderDataType::Float2:    return 2;
        case BHive::EShaderDataType::Float3:    return 3;
        case BHive::EShaderDataType::Float4:    return 4;
        case BHive::EShaderDataType::Int:       return 1;
        case BHive::EShaderDataType::Int2:      return 2;
        case BHive::EShaderDataType::Int3:      return 3;
        case BHive::EShaderDataType::Int4:      return 4;
        case BHive::EShaderDataType::Bool:      return 1;
        case BHive::EShaderDataType::Mat3:      return 3;
        case BHive::EShaderDataType::Mat4:      return 4;
        default:
            break;
        }
        return 0;
    }

    void BufferLayout::CalculateOffsetsAndStride()
    {
        
        for (auto& element : mElements)
        {
            element.Offset = mStride;
            mStride += element.Size;
           
        }
    }
}