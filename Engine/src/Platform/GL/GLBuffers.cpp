#include "GLBuffers.h"
#include <glad/glad.h>
#include "threading/Threading.h"

namespace BHive
{
    GLIndexBuffer::GLIndexBuffer(const uint32_t *data, const uint32_t count)
        : mCount(count)
    {
        BEGIN_THREAD_DISPATCH(=)
        glCreateBuffers(1, &mIndexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, mIndexBufferID);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
        END_THREAD_DISPATCH()
    }

    GLIndexBuffer::GLIndexBuffer(const uint32_t count)
        : mCount(count)
    {
        BEGIN_THREAD_DISPATCH(=)
        glCreateBuffers(1, &mIndexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, mIndexBufferID);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
        END_THREAD_DISPATCH()
    }

    GLIndexBuffer::~GLIndexBuffer()
    {
        BEGIN_THREAD_DISPATCH(=)
        glDeleteBuffers(1, &mIndexBufferID);
        END_THREAD_DISPATCH()
    }

    void GLIndexBuffer::Bind() const
    {
        BEGIN_THREAD_DISPATCH(=)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
        END_THREAD_DISPATCH()
    }

    void GLIndexBuffer::UnBind() const
    {
        BEGIN_THREAD_DISPATCH(=)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        END_THREAD_DISPATCH()
    }

    void GLIndexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
    {
        BEGIN_THREAD_DISPATCH(=)
        glNamedBufferSubData(mIndexBufferID, offset, size, data);
        END_THREAD_DISPATCH()
    }

    GLVertexBuffer::GLVertexBuffer(const float *data, const uint64_t size)
    {
        BEGIN_THREAD_DISPATCH(=)
        glCreateBuffers(1, &mVertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        END_THREAD_DISPATCH()
    }

    GLVertexBuffer::GLVertexBuffer(const uint64_t size)
    {
        BEGIN_THREAD_DISPATCH(=)
        glCreateBuffers(1, &mVertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        END_THREAD_DISPATCH()
    }

    GLVertexBuffer::~GLVertexBuffer()
    {
        BEGIN_THREAD_DISPATCH(=)
        glDeleteBuffers(1, &mVertexBufferID);
        END_THREAD_DISPATCH()
    }

    void GLVertexBuffer::Bind() const
    {
        BEGIN_THREAD_DISPATCH(=)
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
        END_THREAD_DISPATCH()
    }

    void GLVertexBuffer::UnBind() const
    {
        BEGIN_THREAD_DISPATCH(=)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        END_THREAD_DISPATCH()
    }

    void GLVertexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
    {
        BEGIN_THREAD_DISPATCH(=)
        glNamedBufferSubData(mVertexBufferID, offset, size, data);
        END_THREAD_DISPATCH()
    }

    void GLVertexBuffer::SetLayout(const BufferLayout &layout)
    {
        mLayout = layout;
    }
}