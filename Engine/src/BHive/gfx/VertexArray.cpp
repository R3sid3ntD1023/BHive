#include "VertexArray.h"
#include "Platform/GL/GLVertexArray.h"

namespace BHive
{
    Ref<VertexArray> VertexArray::Create()
    {
        return CreateRef<GLVertexArray>();
    }
}