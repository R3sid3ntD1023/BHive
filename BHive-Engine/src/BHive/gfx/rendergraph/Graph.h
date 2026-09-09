#pragma once

#include "core/Core.h"
#include "Commands.h"
#include "Pass.h"

namespace BHive
{
	class Graph
	{
	public:
		FPass &AddPass(const std::string &name, EPassType type, FPassState state = {});

		void Append(Graph &graph);

		bool IsEmpty() const { return mPasses.size() == 0; };

		const auto &GetPasses() const { return mPasses; };

		auto &GetPasses() { return mPasses; };

		void DebugPrint();

	private:
		std::list<FPass> mPasses;
	};

} // namespace BHive