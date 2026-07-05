#pragma once

#include "core/Core.h"
#include "rendergraph/Pass.h"
#include "rendergraph/Commands.h"

namespace BHive
{
	class Texture;
	class Pipeline;
	class BaseMesh;
	class Material;

	class RenderGraph
	{
	public:
	
		FPass &AddPass(const std::string &name, EPassType type, FPassState state = {});

		void Append(const RenderGraph &graph);

		bool Empty() const;

		const std::vector<FPass> &GetPasses() const;

		std::vector<FPass> &GetPasses();

		void DebugPrint();

	private:
		std::vector<FPass> mPasses;
	};

}