#include "GraphSubmission.h"

namespace BHive
{
	void GraphSubmission::Submit(Graph graph)
	{
		if (!graph.IsEmpty())
			mGraph.Append(std::move(graph));
	}

	Graph GraphSubmission::Take()
	{
		mGraph.ResolveBufferTransitions();
		Graph graph = std::move(mGraph);
		mGraph = Graph{};
		return graph;
	}

	void GraphSubmission::Clear()
	{
		mGraph = Graph{};
	}
} // namespace BHive
