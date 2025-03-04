#include "RendererAPI.h"
#include <glad/glad.h>

namespace BHive
{
	const char *get_debug_source_name(unsigned source)
	{
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			return "Window System";
		case GL_DEBUG_SOURCE_APPLICATION:
			return "Application";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			return "Shader Compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			return "Third Party";
		case GL_DEBUG_SOURCE_OTHER:
			return "Other";
		default:
			break;
		}

		ASSERT(false, source);
		return "";
	}

	const char *get_debug_type_name(unsigned type)
	{
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			return "Error";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "Deprecated Behaviour";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "Undefined Behaviour";
		case GL_DEBUG_TYPE_PORTABILITY:
			return "Portability";
		case GL_DEBUG_TYPE_PERFORMANCE:
			return "Performance";
		case GL_DEBUG_TYPE_MARKER:
			return "Marker";
		case GL_DEBUG_TYPE_PUSH_GROUP:
			return "Push Group";
		case GL_DEBUG_TYPE_POP_GROUP:
			return "Pop Group";
		case GL_DEBUG_TYPE_OTHER:
			return "Other";
		default:
			break;
		}
		ASSERT(false, type);
		return "";
	}

	void OpenGLCallback(
		unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char *message,
		const void *userdata)
	{
		auto source_name = get_debug_source_name(source);
		auto type_name = get_debug_type_name(type);

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			LOG_CRITICAL("{} {} {}", source_name, type_name, message);
			ASSERT(false);
			return;
		case GL_DEBUG_SEVERITY_MEDIUM:
			LOG_ERROR("{} {} {}", source_name, type_name, message);
			return;
		case GL_DEBUG_SEVERITY_LOW:
			LOG_WARN("{} {} {}", source_name, type_name, message);
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			LOG_TRACE("{} {} {}", source_name, type_name, message);
			return;
		}
	}

	void RendererAPI::Init()
	{

#if _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_LINE_SMOOTH);
		// glEnable(GL_FRAMEBUFFER_SRGB);
	}

	void RendererAPI::ClearColor(float r, float g, float b, float a)
	{

		glClearColor(r, g, b, a);
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

	void RendererAPI::DrawElementsBaseVertex(
		EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
		glDrawElementsBaseVertex(mode, _count, GL_UNSIGNED_INT, (void *)(sizeof(uint32_t) * start_index), start);
	}

	void
	RendererAPI::DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count)
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

	void RendererAPI::MultiDrawElementsIndirect(
		uint32_t buffer, EDrawMode mode, const VertexArray &vao, size_t numMeshes, size_t stride)
	{
		vao.Bind();

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer);

		glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, NULL, numMeshes, stride);

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
} // namespace BHive
