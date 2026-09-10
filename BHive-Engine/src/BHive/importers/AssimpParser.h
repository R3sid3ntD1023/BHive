#pragma once

#include "core/Core.h"
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace BHive
{
	class AssmipParser
	{
		struct ProgressHandler : public Assimp::ProgressHandler
		{
			bool Update(float percentage = -1) override;
		};

	public:
	};
} // namespace BHive