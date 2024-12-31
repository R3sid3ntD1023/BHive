#include "GLRendererAPI.h"
#include <glad/glad.h>
#include "threading/Threading.h"

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

	void OpenGLCallback(unsigned source,
						unsigned type,
						unsigned id,
						unsigned severity,
						int length,
						const char *message,
						const void *userdata)
	{
		auto source_name = get_debug_source_name(source);
		auto type_name = get_debug_type_name(type);

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			LOG_CRITICAL("{} {} {}", source_name, type_name, message);
			// ASSERT(false);
			return;
		case GL_DEBUG_SEVERITY_MEDIUM:
			LOG_ERROR("{} {} {}", source_name, type_name, message);
			// ASSERT(false);
			return;
		case GL_DEBUG_SEVERITY_LOW:
			LOG_WARN("{} {} {}", source_name, type_name, message);
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			LOG_TRACE("{} {} {}", source_name, type_name, message);
			return;
		}
	}

	void GLRendererAPI::Init()
	{

		BEGIN_THREAD_DISPATCH()
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
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::ClearColor(float r, float g, float b, float a)
	{
		BEGIN_THREAD_DISPATCH(r, g, b, a)
		glClearColor(r, g, b, a);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::Clear(int mask)
	{
		BEGIN_THREAD_DISPATCH(mask)
		glClear(mask);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::SetLineWidth(float width)
	{
		BEGIN_THREAD_DISPATCH(width)
		glLineWidth(width);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		BEGIN_THREAD_DISPATCH(x, y, w, h)
		glViewport(x, y, w, h);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		vao.Bind();
		BEGIN_THREAD_DISPATCH(mode, count)
		glDrawArrays(mode, 0, count);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();
		BEGIN_THREAD_DISPATCH(mode, index_buffer, count)
		auto _count = count ? count : index_buffer->GetCount();
		glDrawElements(mode, _count, GL_UNSIGNED_INT, nullptr);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		BEGIN_THREAD_DISPATCH(mode, index_buffer, start, start_index, count)
		auto _count = count ? count : index_buffer->GetCount();
		glDrawElementsBaseVertex(mode, _count, GL_UNSIGNED_INT, (void *)(sizeof(uint32_t) * start_index), start);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		BEGIN_THREAD_DISPATCH(mode, index_buffer, start, end, count)
		auto _count = count ? count : index_buffer->GetCount();
		glDrawRangeElements(mode, start, end, _count, GL_UNSIGNED_INT, nullptr);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		BEGIN_THREAD_DISPATCH(mode, index_buffer, instances, count)

		auto _count = count ? count : index_buffer->GetCount();
		glDrawElementsInstanced(mode, _count, GL_UNSIGNED_INT, nullptr, instances);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::EnableDepth()
	{
		BEGIN_THREAD_DISPATCH()
		glEnable(GL_DEPTH_TEST);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::DisableDepth()
	{
		BEGIN_THREAD_DISPATCH()
		glDisable(GL_DEPTH_TEST);
		END_THREAD_DISPATCH();
	}

	void GLRendererAPI::DepthFunc(uint32_t func)
	{
		BEGIN_THREAD_DISPATCH(=)
		glDepthFunc(func);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::CullFront()
	{
		BEGIN_THREAD_DISPATCH()
		glCullFace(GL_FRONT);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::CullBack()
	{
		BEGIN_THREAD_DISPATCH()
		glCullFace(GL_BACK);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::SetCullEnabled(bool enabled)
	{
		BEGIN_THREAD_DISPATCH(enabled)
		enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::EnableDepthMask(bool mask)
	{
		BEGIN_THREAD_DISPATCH(mask)
		glDepthMask(mask ? GL_TRUE : GL_FALSE);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::EnableBlend(bool enabled)
	{
		BEGIN_THREAD_DISPATCH(enabled)
		enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
		END_THREAD_DISPATCH()
	}

	void GLRendererAPI::AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer)
	{
		BEGIN_THREAD_DISPATCH(attachment, texture, framebuffer)
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, framebuffer);
		END_THREAD_DISPATCH()
	}

	unsigned GLRendererAPI::CheckError(const char *file, int line)
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
}