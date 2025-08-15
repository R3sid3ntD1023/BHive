#pragma once
#include "Inspectors/InspectorAsset.h"
#include "audio/AudioSource.h"
#include "gfx/textures/Texture2D.h"
#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"
#include "material/Material.h"
#include "sprite/Sprite.h"
#include "sprite/SpriteSheet.h"
#include "sprite/FlipBook.h"
#include "world/World.h"

namespace BHive
{
	REFLECT_EXTERN(Inspector_Asset<Sprite>)
	REFLECT_EXTERN(Inspector_Asset<SpriteSheet>)
	REFLECT_EXTERN(Inspector_Asset<FlipBook>)
	REFLECT_EXTERN(Inspector_Asset<Material>)
	REFLECT_EXTERN(Inspector_Asset<StaticMesh>)
	REFLECT_EXTERN(Inspector_Asset<SkeletalMesh>)
	REFLECT_EXTERN(Inspector_Asset<Texture>)
	REFLECT_EXTERN(Inspector_Asset<Texture2D>)
	REFLECT_EXTERN(Inspector_Asset<AudioSource>)
	REFLECT_EXTERN(Inspector_Asset<World>)
} // namespace BHive