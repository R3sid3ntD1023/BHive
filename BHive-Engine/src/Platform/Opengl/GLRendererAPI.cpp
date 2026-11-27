#include "GLRendererAPI.h"
#include "GLFramebuffer.h"
#include "GLPipeline.h"
#include "GLResourceSet.h"
#include "GLShader.h"
#include "GLVertexArray.h"
#include "gfx/BufferBase.h"
#include "gfx/Buffers.h"
#include "gfx/material/MaterialSnapshot.h"
#include "gfx/rendergraph/Commands.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui_impl_opengl3.h>

namespace BHive
{
	namespace utils
	{
		GLenum GetTextureTarget(ETextureType t)
		{
			switch (t)
			{
			case ETextureType::TEXTURE_1D:
				return GL_TEXTURE_1D;
			case ETextureType::TEXTURE_1D_ARRAY:
				return GL_TEXTURE_1D_ARRAY;
			case ETextureType::TEXTURE_2D:
				return GL_TEXTURE_2D;
			case ETextureType::TEXTURE_2D_ARRAY:
				return GL_TEXTURE_2D_ARRAY;
			case ETextureType::TEXTURE_CUBE:
				return GL_TEXTURE_CUBE_MAP;
			case ETextureType::TEXTURE_CUBE_MAP_ARRAY:
				return GL_TEXTURE_CUBE_MAP_ARRAY;
			case ETextureType::TEXTURE_3D:
				return GL_TEXTURE_3D;
			case ETextureType::TEXTURE_RECTANGLE:
				return GL_TEXTURE_RECTANGLE;
			default:
				return GL_NONE;
			}
		}

		GLenum GetGLFormat(EFormat format)
		{
			switch (format)
			{
			case EFormat::R8:
			case EFormat::R16F:
			case EFormat::R32F:
				return GL_RED;
			case EFormat::RG8:
			case EFormat::RG16F:
			case EFormat::RG32F:
				return GL_RG;
			case EFormat::RGB8:
			case EFormat::RGB16F:
			case EFormat::RGB32F:
			case EFormat::R11_G11_B10:
				return GL_RGB;
			case EFormat::RGBA8:
			case EFormat::RGBA16F:
			case EFormat::RGBA32F:
				return GL_RGBA;
			case EFormat::RED_INTEGER:
				return GL_RED_INTEGER;
			case EFormat::RGB_INTEGER:
			case EFormat::RGB_UINTEGER:
				return GL_RGB_INTEGER;
			case EFormat::RGBA_INTEGER:
				return GL_RGBA_INTEGER;
			case EFormat::DEPTH_COMPONENT:
			case EFormat::DEPTH_COMPONENT_24:
			case EFormat::DEPTH_COMPONENT_32F:
				return GL_DEPTH_COMPONENT;
			case EFormat::DEPTH24_STENCIL8:
				return GL_DEPTH_STENCIL;

			default:
				break;
			}

			return GL_NONE;
		}

		GLenum GetGLMagFilter(EMagFilter f)
		{
			switch (f)
			{
			case EMagFilter::NEAREST:
				return GL_NEAREST;
			default:
				return GL_LINEAR;
			}
		}

		GLenum GetGLMinFilter(EMinFilter f)
		{
			switch (f)
			{
			case EMinFilter::NEAREST:
				return GL_NEAREST;
			case EMinFilter::LINEAR:
				return GL_LINEAR;
			case EMinFilter::MIPMAP_LINEAR:
				return GL_LINEAR_MIPMAP_LINEAR;
			case EMinFilter::MIPMAP_NEAREST:
				return GL_NEAREST_MIPMAP_NEAREST;
			case EMinFilter::MIPMAP_LINEAR_NEAREST:
				return GL_LINEAR_MIPMAP_NEAREST;
			case EMinFilter::MIPMAP_NEAREST_LINEAR:
				return GL_NEAREST_MIPMAP_LINEAR;
			default:
				return GL_LINEAR;
			}
		}

		GLenum GetGLWarpMode(EWrapMode w)
		{
			switch (w)
			{
			case EWrapMode::REPEAT:
				return GL_REPEAT;
			case EWrapMode::MIRRORED_REPEAT:
				return GL_MIRRORED_REPEAT;
			case EWrapMode::CLAMP_TO_EDGE:
				return GL_CLAMP_TO_EDGE;
			case EWrapMode::CLAMP_TO_BORDER:
				return GL_CLAMP_TO_BORDER;
			default:
				return GL_REPEAT;
			}
		}

		GLenum GetGLCompareMode(ECompareMode m)
		{
			switch (m)
			{
			case ECompareMode::COMPARE_REF_TO_TEXTURE:
				return GL_COMPARE_REF_TO_TEXTURE;
			default:
				return GL_NONE;
			}
		}

		GLenum GetGLCompareOp(ECompareOp o)
		{
			switch (o)
			{
			case ECompareOp::Never:
				return GL_NEVER;
			case ECompareOp::Less:
				return GL_LESS;
			case ECompareOp::Equal:
				return GL_EQUAL;
			case ECompareOp::LessOrEqual:
				return GL_LEQUAL;
			case ECompareOp::Greater:
				return GL_GREATER;
			case ECompareOp::NotEqual:
				return GL_NOTEQUAL;
			case ECompareOp::GreaterOrEqual:
				return GL_GEQUAL;
			case ECompareOp::Always:
				return GL_ALWAYS;
			default:
				return GL_NONE;
			}
		}

		GLint GetGLInternalFormat(EFormat f)
		{
			switch (f)
			{
			case EFormat::R8:
				return GL_R8;
			case EFormat::R16F:
				return GL_R16F;
			case EFormat::R32F:
				return GL_R32F;
			case EFormat::RG8:
				return GL_RG8;
			case EFormat::RG16F:
				return GL_RG16F;

			case EFormat::RG32F:
				return GL_RG32F;
			case EFormat::RGB8:
				return GL_RGB8;
			case EFormat::RGB16F:
				return GL_RGB16F;
			case EFormat::RGB32F:
				return GL_RGB32F;
			case EFormat::RGBA8:
				return GL_RGBA8;
			case EFormat::RGBA16F:
				return GL_RGBA16F;
			case EFormat::RGBA32F:
				return GL_RGBA32F;
			case EFormat::RED_INTEGER:
				return GL_R32I;
			case EFormat::RGB_INTEGER:
				return GL_RGB32I;
			case EFormat::RGB_UINTEGER:
				return GL_RGB32UI;
			case EFormat::RGBA_INTEGER:
				return GL_RGBA32I;
			case EFormat::R11_G11_B10:
				return GL_R11F_G11F_B10F;
			case EFormat::DEPTH24_STENCIL8:
				return GL_DEPTH24_STENCIL8;
			case EFormat::DEPTH_COMPONENT:
				return GL_DEPTH_COMPONENT24;
			case EFormat::DEPTH_COMPONENT_32F:
				return GL_DEPTH_COMPONENT32F;
			case EFormat::DEPTH_COMPONENT_24:
				return GL_DEPTH_COMPONENT24;
			default:
				break;
			}

			return GL_NONE;
		}

		GLenum GetGLType(EFormat f)
		{
			switch (f)
			{
			case EFormat::DEPTH_COMPONENT:
			case EFormat::DEPTH_COMPONENT_24:
			case EFormat::DEPTH_COMPONENT_32F:
				return GL_FLOAT;

			case EFormat::DEPTH24_STENCIL8:
				return GL_UNSIGNED_INT_24_8;

			case EFormat::RED_INTEGER:
			case EFormat::RGB_INTEGER:
			case EFormat::RGBA_INTEGER:
				return GL_INT;
			case EFormat::RGB_UINTEGER:
				return GL_UNSIGNED_INT;
			case EFormat::R8:
			case EFormat::RG8:
			case EFormat::RGB8:
			case EFormat::RGBA8:
				return GL_UNSIGNED_BYTE;
			case EFormat::R16F:
			case EFormat::RG16F:
			case EFormat::RGB16F:
			case EFormat::RGBA16F:
				return GL_HALF_FLOAT;
			case EFormat::R32F:
			case EFormat::RG32F:
			case EFormat::RGB32F:
			case EFormat::RGBA32F:
				return GL_FLOAT;
			case EFormat::R11_G11_B10:
				return GL_UNSIGNED_INT_10F_11F_11F_REV;

			default:
				break;
			}

			return GL_NONE;
		}

		glm::vec4 GetBorderColor(EBorderColor c)
		{
			switch (c)
			{
			case EBorderColor::FLOAT_OPAQUE_BLACK:
				return {0.0f, 0.0f, 0.0f, 1.0f};
			case EBorderColor::FLOAT_OPAQUE_WHITE:
				return {1.0f, 1.0f, 1.0f, 1.0f};
			case EBorderColor::FLOAT_TRANSPARENT_BLACK:
				return {0.0f, 0.0f, 0.0f, 0.0f};
			case EBorderColor::INT_OPAQUE_BLACK:
				return {0.0f, 0.0f, 0.0f, 1.0f};
			case EBorderColor::INT_OPAQUE_WHITE:
				return {1.0f, 1.0f, 1.0f, 1.0f};
			case EBorderColor::INT_TRANSPARENT_BLACK:
				return {0.0f, 0.0f, 0.0f, 0.0f};
			default:
				break;
			}

			return {0, 0, 0, 0};
		}

		GLTextureInfo GetGLInfo(FTextureCreateInfo info)
		{
			GLTextureInfo out{};
			out.Format = GetGLFormat(info.Format);
			out.MagFilter = GetGLMagFilter(info.MagFilter);
			out.MinFilter = GetGLMinFilter(info.MinFilter);
			out.WrapMode = GetGLWarpMode(info.WrapMode);
			out.CompareMode = info.CompareMode.has_value() ? GetGLCompareMode(info.CompareMode.value()) : GL_NONE;
			out.CompareOp = info.CompareMode.has_value() ? GetGLCompareOp(info.CompareOp.value()) : GL_NONE;
			out.InternalFormat = GetGLInternalFormat(info.Format);
			out.Type = GetGLType(info.Format);
			out.MipLevels = info.MipLevels;
			out.BorderColor = GetBorderColor(info.BorderColor);
			return out;
		}

		GLenum ToGLTopology(ETopologyMode mode)
		{
			switch (mode)
			{
			case ETopologyMode::Points:
				return GL_POINTS;
			case ETopologyMode::Lines:
				return GL_LINES;
			case ETopologyMode::LineStrip:
				return GL_LINE_STRIP;
			case ETopologyMode::Triangles:
				return GL_TRIANGLES;
			case ETopologyMode::TriangleStrip:
				return GL_TRIANGLE_STRIP;
			case ETopologyMode::TriangleFan:
				return GL_TRIANGLE_FAN;
			default:
				return GL_TRIANGLES;
			}
		}

	} // namespace utils

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

		ASSERT(false, "{}", source);
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
		ASSERT(false, "{}", type);
		return "";
	}

	void OpenGLCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char *message, const void *userdata)
	{
		auto source_name = get_debug_source_name(source);
		auto type_name = get_debug_type_name(type);

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			LOG_CRITICAL("{} {} {} {}", id, source_name, type_name, message);
			return;
		case GL_DEBUG_SEVERITY_MEDIUM:
			LOG_ERROR("{} {} {} {}", id, source_name, type_name, message);
			return;
		case GL_DEBUG_SEVERITY_LOW:
			LOG_WARN("{} {} {} {}", id, source_name, type_name, message);
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			LOG_TRACE("{} {} {} {}", id, source_name, type_name, message);
			return;
		}
	}

	GLRendererAPI::GLRendererAPI()
	{
	}

	GLRendererAPI::~GLRendererAPI()
	{
	}

	void GLRendererAPI::Init()
	{
		glCreateVertexArrays(1, &mFullscreenVertexArray);

		if (EngineConfig::DebugEnabled)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		}
	}

	void GLRendererAPI::Shutdown()
	{
		if (mFullscreenVertexArray)
		{
			glDeleteVertexArrays(1, &mFullscreenVertexArray);
			mFullscreenVertexArray = 0;
		}
	}

	void GLRendererAPI::SubmitGraph(const Graph &graph)
	{
		if (!graph.IsEmpty())
			mSubmittedGraphs.emplace_back(graph);
	}

	void GLRendererAPI::RenderFrame()
	{
		Graph finalGraph;
		for (auto &g : mSubmittedGraphs)
			finalGraph.Append(g);

		mSubmittedGraphs.clear();

		if (finalGraph.IsEmpty())
		{
			return;
		}

		return ExecuteFinalGraph(finalGraph);
	}

	void GLRendererAPI::QueueDeletion(FQeueuDeletionFunc &&fn)
	{
	}

	void GLRendererAPI::CreateTexture(uint32_t &id, ETextureType type, const glm::uvec3 &size, const FTextureCreateInfo &info)
	{
		GLTextureInfo glInfo = utils::GetGLInfo(info);
		glInfo.Target = utils::GetTextureTarget(type);
		GLenum target = glInfo.Target;

		if (target == GL_NONE || glInfo.InternalFormat == GL_NONE || glInfo.MipLevels == 0)
		{
			LOG_ERROR("Cannot create OpenGL texture with type {} and format {}", (uint32_t)type, (uint32_t)info.Format);
			id = 0;
			return;
		}
		if (type == ETextureType::TEXTURE_RECTANGLE && glInfo.MipLevels != 1)
		{
			LOG_ERROR("OpenGL rectangle textures cannot have mip levels");
			id = 0;
			return;
		}
		if ((type == ETextureType::TEXTURE_1D_ARRAY || type == ETextureType::TEXTURE_2D_ARRAY || type == ETextureType::TEXTURE_CUBE_MAP_ARRAY) && info.ArrayLayers == 0)
		{
			LOG_ERROR("Cannot create an OpenGL array texture with zero layers");
			id = 0;
			return;
		}

		glCreateTextures(target, 1, &id);

		switch (type)
		{
		case ETextureType::TEXTURE_1D:
			glTextureStorage1D(id, glInfo.MipLevels, glInfo.InternalFormat, size.x);
			break;
		case ETextureType::TEXTURE_1D_ARRAY:
			glTextureStorage2D(id, glInfo.MipLevels, glInfo.InternalFormat, size.x, info.ArrayLayers);
			break;
		case ETextureType::TEXTURE_2D:
			glTextureStorage2D(id, glInfo.MipLevels, glInfo.InternalFormat, size.x, size.y);
			break;
		case ETextureType::TEXTURE_2D_ARRAY:
			glTextureStorage3D(id, glInfo.MipLevels, glInfo.InternalFormat, size.x, size.y, info.ArrayLayers);
			break;
		case ETextureType::TEXTURE_CUBE:
			glTextureStorage2D(id, glInfo.MipLevels, glInfo.InternalFormat, size.x, size.y);
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			break;
		case ETextureType::TEXTURE_CUBE_MAP_ARRAY:
			glTextureStorage3D(id, glInfo.MipLevels, glInfo.InternalFormat, size.x, size.y, info.ArrayLayers * 6);
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			break;
		case ETextureType::TEXTURE_3D:
			glTextureStorage3D(id, glInfo.MipLevels, glInfo.InternalFormat, size.x, size.y, size.z);
			break;
		case ETextureType::TEXTURE_RECTANGLE:
			glTextureStorage2D(id, 1, glInfo.InternalFormat, size.x, size.y);
			break;
		default:
			glDeleteTextures(1, &id);
			id = 0;
			return;
		}

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, glInfo.WrapMode);
		if (target != GL_TEXTURE_1D)
			glTextureParameteri(id, GL_TEXTURE_WRAP_T, glInfo.WrapMode);
		if (target == GL_TEXTURE_3D || target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY)
			glTextureParameteri(id, GL_TEXTURE_WRAP_R, glInfo.WrapMode);

		glTextureParameterfv(id, GL_TEXTURE_BORDER_COLOR, &glInfo.BorderColor.x);

		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, glInfo.MinFilter);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, glInfo.MagFilter);
		if (info.CompareMode.has_value())
			glTextureParameteri(id, GL_TEXTURE_COMPARE_MODE, *glInfo.CompareMode);
		if (info.CompareOp.has_value())
			glTextureParameteri(id, GL_TEXTURE_COMPARE_FUNC, *glInfo.CompareOp);

		mTextureInfos[id] = glInfo;
	}

	void GLRendererAPI::DeleteTexture(uint32_t &textureID)
	{
		glDeleteTextures(1, &textureID);
		mTextureInfos.erase(textureID);
	}

	void GLRendererAPI::SetTextureData(uint32_t textureID, const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range)
	{
		if (textureID == 0)
			return;

		auto it = mTextureInfos.find(textureID);
		if (it == mTextureInfos.end())
			return;

		auto &info = it->second;

		for (uint32_t level = range.BaseMipLevel; level < range.BaseMipLevel + range.LevelCount; ++level)
		{
			switch (info.Target)
			{
			case GL_TEXTURE_1D:
				glTextureSubImage1D(textureID, level, region.Offset.x, region.Extents.x, info.Format, info.Type, data);
				break;

			case GL_TEXTURE_1D_ARRAY:
				glTextureSubImage2D(
					textureID,
					level,
					region.Offset.x,
					static_cast<GLint>(region.BaseArrayLayer) + region.Offset.y,
					region.Extents.x,
					region.LayerCount,
					info.Format,
					info.Type,
					data
				);
				break;

			case GL_TEXTURE_2D:
			case GL_TEXTURE_RECTANGLE:
				glTextureSubImage2D(textureID, level, region.Offset.x, region.Offset.y, region.Extents.x, region.Extents.y, info.Format, info.Type, data);
				break;

			case GL_TEXTURE_2D_ARRAY:
			case GL_TEXTURE_CUBE_MAP_ARRAY:
				glTextureSubImage3D(
					textureID,
					level,
					region.Offset.x,
					region.Offset.y,
					static_cast<GLint>(region.BaseArrayLayer) + region.Offset.z,
					region.Extents.x,
					region.Extents.y,
					region.LayerCount,
					info.Format,
					info.Type,
					data
				);
				break;

			case GL_TEXTURE_CUBE_MAP:
				for (uint32_t layer = 0; layer < region.LayerCount; ++layer)
				{
					const GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + region.BaseArrayLayer + layer;
					glTextureSubImage2DEXT(textureID, face, level, region.Offset.x, region.Offset.y, region.Extents.x, region.Extents.y, info.Format, info.Type, data);
				}
				break;

			case GL_TEXTURE_3D:
				glTextureSubImage3D(
					textureID, level, region.Offset.x, region.Offset.y, region.Offset.z, region.Extents.x, region.Extents.y, region.Extents.z, info.Format, info.Type, data
				);
				break;
			default:
				break;
			}
		}
	}

	void GLRendererAPI::CreateBuffer(uint32_t &id, EBufferType type, EBufferLifetime lifeTime, size_t size, const char *debugName)
	{
		glCreateBuffers(1, &id);

		auto usage = lifeTime == EBufferLifetime::Static ? GL_STATIC_DRAW : GL_STREAM_DRAW;

		glNamedBufferData(id, size, nullptr, usage);
	}

	void GLRendererAPI::DeleteBuffer(uint32_t &bufferID)
	{
		glDeleteBuffers(1, &bufferID);
	}

	void GLRendererAPI::SetBufferData(uint32_t bufferID, const void *data, size_t size, uint32_t offset)
	{
		if (bufferID != 0 && data && size != 0)
			glNamedBufferSubData(bufferID, offset, size, data);
	}

	void GLRendererAPI::ClearBuffer(uint32_t bufferID)
	{
		if (!bufferID)
			return;

		const GLuint zero = 0;
		glClearNamedBufferData(bufferID, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
	}

	void GLRendererAPI::ExecuteFinalGraph(Graph &graph)
	{
		for (auto &p : graph.GetPasses())
		{
			const auto &s = p.State;

			for (auto &ph : p.Phases)
			{
				if (ph.Type == EPhaseType::Graphics)
				{
					if (p.Type == EPassType::OffScreen)
					{
						BeginOffScreenRendering(s, ph);
					}
					else
					{
						glBindFramebuffer(GL_FRAMEBUFFER, 0);

						if (GLFWwindow *window = glfwGetCurrentContext())
						{
							int width = 0;
							int height = 0;
							glfwGetFramebufferSize(window, &width, &height);
							glViewport(0, 0, width, height);
						}

						if (s.Color.LoadOP == EAttachmentLoadState::Clear)
						{
							const auto &color = s.Color.ClearColor;
							glClearColor(color.r, color.g, color.b, color.a);
							glClear(GL_COLOR_BUFFER_BIT);
						}
						if (s.Depth.LoadOP == EAttachmentLoadState::Clear)
						{
							glClearDepth(1.0);
							glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
						}
					}
				}

				ExecuteCommands(ph);
				if (ph.Type != EPhaseType::Transfer)
					glMemoryBarrier(GL_ALL_BARRIER_BITS);

				if (ph.Type == EPhaseType::Graphics)
				{
					if (p.Type == EPassType::OffScreen)
					{
						EndOffScreenRendering(s, ph);
					}
				}
			}
		}
	}

	void GLRendererAPI::BeginOffScreenRendering(const FPassState &state, const FPhase &phase)
	{
		if (!phase.BoundFBO.FBO)
			return;

		auto fbo = phase.BoundFBO.FBO.As<GLFramebuffer>();
		fbo->BeginRendering(phase.BoundFBO.Range);

		auto color = state.Color.ClearColor;
		if (state.Depth.LoadOP == EAttachmentLoadState::Clear)
		{
			glClearDepth(1.0);
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}
		if (state.Color.LoadOP == EAttachmentLoadState::Clear)
		{
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}

	void GLRendererAPI::EndOffScreenRendering(const FPassState &state, const FPhase &phase)
	{
		if (!phase.BoundFBO.FBO)
			return;

		auto fbo = phase.BoundFBO.FBO.As<GLFramebuffer>();
		fbo->EndRendering(phase.BoundFBO.Range);
	}

	void GLRendererAPI::ExecuteCommands(const FPhase &phase)
	{
		if (!phase.Commands)
			return;

		if (!phase.Images.empty() || !phase.BufferTransitions.empty())
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

		auto &commands = *phase.Commands;
		const auto framebuffer = phase.BoundFBO.FBO;
		const uint32_t numAttachments = framebuffer ? framebuffer.As<Framebuffer>()->GetNumColorAttachments() : 0;
		size_t offset = 0;

		while (offset < commands.GetUsedSize())
		{
			const auto *record = commands.Data() + offset;
			const auto *header = reinterpret_cast<const CmdHeader *>(record);
			const auto *payload = record + header->PayloadOffset;

			switch (header->Type)
			{
			case ECommandType::GenerateMipMaps:
			{
				const auto &command = *reinterpret_cast<const CmdGenerateMipMaps *>(payload);
				if (command.Texture > 0)
					glGenerateTextureMipmap(static_cast<GLuint>(command.Texture));
				break;
			}
			case ECommandType::BindPipeline:
			{
				const auto &command = *reinterpret_cast<const CmdBindPipeline *>(payload);
				if (auto pipeline = command.Pipeline.As<GLPipeline>())
					pipeline->Bind(numAttachments);
				else
					LOG_ERROR("OpenGL received an invalid pipeline handle");
				break;
			}
			case ECommandType::BindVertexArray:
			{
				const auto &command = *reinterpret_cast<const CmdBindVertexArray *>(payload);
				if (auto vertexArray = command.VertexArray.As<GLVertexArray>())
					vertexArray->Bind();
				else
					LOG_ERROR("OpenGL received an invalid vertex-array handle");
				break;
			}
			case ECommandType::BindMaterial:
			{
				const auto &command = *reinterpret_cast<const CmdBindMaterial *>(payload);
				const auto &snapshot = commands.GetObject<MaterialSnapshot>(command.SnapshotIndex);
				auto shader = snapshot.Shader.As<GLShader>();
				if (!shader)
				{
					LOG_ERROR("OpenGL material snapshot has an invalid shader handle");
					break;
				}

				shader->Bind();
				for (const auto &resourceSetHandle : phase.ResourceSets)
				{
					auto resourceSet = resourceSetHandle.As<GLResourceSet>();
					if (resourceSet)
						shader->BindSet(resourceSet);
					else
						LOG_ERROR("OpenGL phase contains an invalid resource-set handle");
				}

				if (const auto *setTemplate = shader->GetTemplate().FindSet(ERenderingSets::MATERIAL))
				{
					GLResourceSet materialSet(*setTemplate);
					for (const auto &[binding, buffer] : snapshot.Buffers)
						materialSet.SetBuffer(binding, buffer.Buffer);
					for (const auto &[binding, texture] : snapshot.Textures)
						materialSet.SetTexture(binding, texture.Texture, texture.BaseMipLevel);
					shader->BindSet(&materialSet);
				}

				if (!snapshot.PushConstantData.empty())
					shader->SetPushConstants(snapshot.PushConstantData);

				if (HasFlag(snapshot.Flags, EMaterialFlags::DoubleSided))
				{
					glDisable(GL_CULL_FACE);
				}
				break;
			}
			case ECommandType::UploadBuffer:
			{
				const auto &command = *reinterpret_cast<const CmdUploadBuffer *>(payload);
				if (auto buffer = command.Buffer.As<BufferBase>())
					SetBufferData(buffer->GetBufferID(), commands.GetData(command.DataOffset), command.Size, command.Offset);
				else
					LOG_ERROR("OpenGL buffer upload has an invalid buffer handle");
				break;
			}
			case ECommandType::SetBufferData:
			{
				const auto &command = *reinterpret_cast<const CmdSetBufferData *>(payload);
				if (auto buffer = command.Buffer.As<BufferBase>())
					SetBufferData(buffer->GetBufferID(), commands.GetData(command.DataOffset), command.Size, command.Offset);
				else
					LOG_ERROR("OpenGL buffer update has an invalid buffer handle");
				break;
			}
			case ECommandType::ClearBuffer:
			{
				const auto &command = *reinterpret_cast<const CmdClearBuffer *>(payload);
				if (auto buffer = command.Buffer.As<BufferBase>())
					ClearBuffer(buffer->GetBufferID());
				else
					LOG_ERROR("OpenGL buffer clear has an invalid buffer handle");
				break;
			}
			case ECommandType::Draw:
			{
				const auto &command = *reinterpret_cast<const CmdDraw *>(payload);
				if (command.VAO)
				{
					if (auto vertexArray = command.VAO.As<GLVertexArray>())
						vertexArray->Bind();
					else
					{
						LOG_ERROR("OpenGL draw has an invalid vertex-array handle");
						break;
					}
				}
				else
				{
					glBindVertexArray(mFullscreenVertexArray);
				}

				glDrawArrays(utils::ToGLTopology(command.Mode), 0, static_cast<GLsizei>(command.Count));
				break;
			}
			case ECommandType::DrawIndexed:
			{
				const auto &command = *reinterpret_cast<const CmdDrawIndexed *>(payload);
				auto vertexArray = command.VAO.As<GLVertexArray>();
				if (!vertexArray)
				{
					LOG_ERROR("OpenGL indexed draw has an invalid vertex-array handle");
					break;
				}

				const auto indexBufferHandle = vertexArray->GetIndexBuffer();
				auto indexBuffer = indexBufferHandle.As<IndexBuffer>();
				if (!indexBuffer)
				{
					LOG_ERROR("OpenGL indexed draw requires an index buffer");
					break;
				}

				vertexArray->Bind();
				const uint32_t count = command.Count ? command.Count : indexBuffer->GetCount();
				glDrawElements(utils::ToGLTopology(command.Mode), static_cast<GLsizei>(count), GL_UNSIGNED_INT, nullptr);
				break;
			}
			case ECommandType::MultiDrawIndexedIndirect:
			{
				const auto &command = *reinterpret_cast<const CmdMultiDrawIndexedIndirect *>(payload);
				auto buffer = command.Buffer.As<BufferBase>();
				if (!buffer)
				{
					LOG_ERROR("OpenGL indirect draw has an invalid buffer handle");
					break;
				}

				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->GetBufferID());
				const auto *drawOffset = reinterpret_cast<const void *>(static_cast<uintptr_t>(command.Offset));
				glMultiDrawElementsIndirect(
					utils::ToGLTopology(command.Mode), GL_UNSIGNED_INT, drawOffset, static_cast<GLsizei>(command.DrawCount), static_cast<GLsizei>(command.Stride)
				);
				break;
			}
			case ECommandType::SetLineWidth:
			{
				const auto &command = *reinterpret_cast<const CmdSetLineWidth *>(payload);
				glLineWidth(command.Width);
				break;
			}
			case ECommandType::Dispatch:
			{
				const auto &command = *reinterpret_cast<const CmdDispatch *>(payload);
				glDispatchCompute(command.X, command.Y, command.Z);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
				break;
			}
			case ECommandType::ImGuiRender:
			{
				const auto &command = *reinterpret_cast<const CmdImGuiRender *>(payload);
				if (command.DrawData)
					ImGui_ImplOpenGL3_RenderDrawData(command.DrawData);
				break;
			}
			case ECommandType::DrawFullScreen:
			{

				glBindVertexArray(mFullscreenVertexArray);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				break;
			}
			default:
				LOG_ERROR("OpenGL received unsupported render command type {}", static_cast<uint32_t>(header->Type));
				break;
			}

			offset += header->RecordSize;
		}
	}
} // namespace BHive