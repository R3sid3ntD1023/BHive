#include "Layer.h"

namespace BHive
{
	LayerStack::~LayerStack()
	{
		for (auto layer : mLayers)
		{
			layer->OnDetach();
		}
	}

	void LayerStack::Push(const Ref<Layer> &layer)
	{
		mLayers.emplace_back(layer);
	}

	void LayerStack::Pop(const Ref<Layer> &layer)
	{
		auto it = std::find(begin(), end(), layer);

		if (it != end())
		{
			mLayers.erase(it);
		}
	}
}