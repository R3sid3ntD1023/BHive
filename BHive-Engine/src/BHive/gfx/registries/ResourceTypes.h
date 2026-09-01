#pragma once

#include "core/type/TypeName.h"

namespace BHive
{
	class Skeleton;
	class Material;
	class BaseMesh;
	class Skeleton;
	class SkeletalAnimation;
	class Sprite;
	class SpriteSheet;
	class FlipBook;
	class Font;
	class Texture;
	class Framebuffer;
	class VertexArray;
	class BufferBase;
	class IndexBuffer;
	class VertexBuffer;
	class Shader;
	class Pipeline;

	REGISTER_TYPE(Material)
	REGISTER_TYPE(Texture)
	REGISTER_TYPE(BaseMesh)
	REGISTER_TYPE(Skeleton)
	REGISTER_TYPE(SkeletalAnimation)
	REGISTER_TYPE(Sprite)
	REGISTER_TYPE(FlipBook)
	REGISTER_TYPE(SpriteSheet)
	REGISTER_TYPE(Font)
	REGISTER_TYPE(Framebuffer)
	REGISTER_TYPE(VertexArray)
	REGISTER_TYPE(VertexBuffer)
	REGISTER_TYPE(IndexBuffer)
	REGISTER_TYPE(BufferBase)
	REGISTER_TYPE(Shader)
	REGISTER_TYPE(Pipeline)
} // namespace BHive