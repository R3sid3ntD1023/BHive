#include "GLBuffers.h"
#include <glad/glad.h>
#include "threading/Threading.h"

namespace BHive
{
    GLIndexBuffer::GLIndexBuffer(const uint32_t *data, const uint32_t count)
        : mCount(count)
    {

        glCreateBuffers(1, &mIndexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, mIndexBufferID);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
    }

    GLIndexBuffer::GLIndexBuffer(const uint32_t count)
        : mCount(count)
    {

        glCreateBuffers(1, &mIndexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, mIndexBufferID);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
    }

    GLIndexBuffer::~GLIndexBuffer()
    {

        glDeleteBuffers(1, &mIndexBufferID);
    }

    void GLIndexBuffer::Bind() const
    {

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
    }

    void GLIndexBuffer::UnBind() const
    {

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void GLIndexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
    {

        glNamedBufferSubData(mIndexBufferID, offset, size, data);
    }

    GLVertexBuffer::GLVertexBuffer(const float *data, const uint64_t size)
    {

        glCreateBuffers(1, &mVertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    GLVertexBuffer::GLVertexBuffer(const uint64_t size)
    {

        glCreateBuffers(1, &mVertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    }

    GLVertexBuffer::~GLVertexBuffer()
    {

        glDeleteBuffers(1, &mVertexBufferID);
    }

    void GLVertexBuffer::Bind() const
    {

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    }

    void GLVertexBuffer::UnBind() const
    {

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void GLVertexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
    {

        glNamedBufferSubData(mVertexBufferID, offset, size, data);
    }

    void GLVertexBuffer::SetLayout(const BufferLayout &layout)
    {
        mLayout = layout;
    }
}