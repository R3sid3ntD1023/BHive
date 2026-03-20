#pragma once
#include "audio/AudioSource.h"
#include "gfx/Texture.h"
#include "Inspectors/InspectorAsset.h"
#include "gfx/material/Material.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/mesh/StaticMesh.h"
#include "physics/PhysicsMaterial.h"
#include "gfx/sprite/FlipBook.h"
#include "gfx/sprite/Sprite.h"
#include "gfx/sprite/SpriteSheet.h"
#include "runtime/World.h"

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
	REFLECT_EXTERN(Inspector_Asset<PhysicsMaterial>)
} // namespace BHive