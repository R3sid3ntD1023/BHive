#pragma once

#include "ResourceHandle.h"

namespace BHive
{
	class BaseMesh;
	class Texture;
	class Material;
	class Sprite;
	class FlipBook;
	class SpriteSheet;
	class Texture3D;
	class Texture2D;
	class Texture2DArray;
	class TextureCube;
	class TextureCubeArray;
	class Framebuffer;
	class Pipeline;
	class Shader;
	class IndexBuffer;
	class VertexBuffer;
	class GeneralBuffer;
	class StaticMesh;
	class SkeletalMesh;
	class SkeletalAnimation;
	class Skeleton;
	class Font;
	class VertexArray;
	class BufferBase;

	using MeshPtr = ResourceHandle;

	using StaticMeshPtr = ResourceHandle;

	using SkeletalMeshPtr = ResourceHandle;

	using SkeletonPtr = ResourceHandle;

	using SkeletalAnimationPtr = ResourceHandle;

	using TexturePtr = ResourceHandle;

	using Texture2DPtr = ResourceHandle;

	using Texture2DArrayPtr = ResourceHandle;

	using Texture3DPtr = ResourceHandle;

	using TextureCubePtr = ResourceHandle;

	using TextureCubeArrayPtr = ResourceHandle;

	using MaterialPtr = ResourceHandle;

	using SpritePtr = ResourceHandle;

	using FlipBookPtr = ResourceHandle;

	using SpriteSheetPtr = ResourceHandle;

	using FontPtr = ResourceHandle;

	using FramebufferPtr = ResourceHandle;

	using PipelinePtr = ResourceHandle;

	using ShaderPtr = ResourceHandle;

	using BufferPtr = ResourceHandle;

	using VertexArrayPtr = ResourceHandle;

	using IndexBufferPtr = ResourceHandle;

	using VertexBufferPtr = ResourceHandle;

	using GeneralBufferPtr = ResourceHandle;
} // namespace BHive
