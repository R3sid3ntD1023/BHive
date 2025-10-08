#include "RendererAPI.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "core/Application.h"
#include "GraphicsContext.h"
#include "VulkanUtils.h"

namespace BHive
{


	RendererAPI::~RendererAPI()
	{
		
	}

	void RendererAPI::BeginFrame()
	{
		auto &cmd = GraphicsContext::Get().GetCommandBuffer();
	}

	void RendererAPI::EndFrame()
	{
		auto &cmd = GraphicsContext::Get().GetCommandBuffer();
	}

	void RendererAPI::Init()
	{
	}

	void RendererAPI::Shutdown()
	{
	}

	void RendererAPI::ClearColor(float r, float g, float b, float a)
	{
		mCurrentClearColor = {r, g, b, a};
	}

	void RendererAPI::Clear(int mask)
	{

		glClear(mask);
	}

	void RendererAPI::SetLineWidth(float width)
	{

		glLineWidth(width);
	}

	void RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{

		glViewport(x, y, w, h);
	}

	void RendererAPI::DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		vao.Bind();

		glDrawArrays(mode, 0, count);
	}

	void RendererAPI::DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
		glDrawElements(mode, _count, GL_UNSIGNED_INT, nullptr);
	}

	void RendererAPI::DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();

		if (instance_count > 0)
			glDrawElementsInstancedBaseVertexBaseInstance(mode, _count, GL_UNSIGNED_INT, nullptr, (GLsizei)instance_count, (GLint)start, 1);
		else
			glDrawElementsBaseVertex(mode, _count, GL_UNSIGNED_INT, nullptr, start);
	}

	void RendererAPI::DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
		glDrawRangeElements(mode, start, end, _count, GL_UNSIGNED_INT, nullptr);
	}

	void RendererAPI::DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
		glDrawElementsInstanced(mode, _count, GL_UNSIGNED_INT, nullptr, instances);
	}

	void RendererAPI::MultiDrawElementsIndirect(EDrawMode mode, const BufferBase &indirect, const VertexArray &vao, const void *data, size_t drawCount, size_t stride)
	{
		vao.Bind();

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect.GetBufferID());

		glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, nullptr, drawCount, stride);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		vao.UnBind();
	}

	void RendererAPI::EnableDepth()
	{

		glEnable(GL_DEPTH_TEST);
	}

	void RendererAPI::DisableDepth()
	{

		glDisable(GL_DEPTH_TEST);
	}

	void RendererAPI::DepthFunc(uint32_t func)
	{

		glDepthFunc(func);
	}

	void RendererAPI::CullFront()
	{

		glCullFace(GL_FRONT);
	}

	void RendererAPI::CullBack()
	{

		glCullFace(GL_BACK);
	}

	void RendererAPI::SetCullEnabled(bool enabled)
	{

		enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	}

	void RendererAPI::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		glColorMask(r, g, b, a);
	}

	void RendererAPI::EnableDepthMask(bool mask)
	{

		glDepthMask(mask ? GL_TRUE : GL_FALSE);
	}

	void RendererAPI::EnableBlend(bool enabled)
	{

		enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	}

	void RendererAPI::AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer)
	{

		glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, framebuffer);
	}

	unsigned RendererAPI::CheckError(const char *file, int line)
	{
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (errorCode)
			{
			case GL_INVALID_ENUM:
				error = "INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				error = "INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				error = "INVALID_OPERATION";
				break;
			case GL_STACK_OVERFLOW:
				error = "STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				error = "STACK_UNDERFLOW";
				break;
			case GL_OUT_OF_MEMORY:
				error = "OUT_OF_MEMORY";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				error = "INVALID_FRAMEBUFFER_OPERATION";
				break;
			}

			LOG_ERROR("{} - file: {}, line: {} ", error, file, line);
		}
		return errorCode;
	}

	void *RendererAPI::CreateShader(const uint32_t *data, size_t size)
	{
		vk::ShaderModuleCreateInfo create_info({}, size, data);
		auto shader_module = VulkanUtils::CreateShaderModule(create_info);
		mVulkanShaders.push_back(shader_module);
		return &mVulkanShaders.back();
		return nullptr;
	}


} // namespace BHive
