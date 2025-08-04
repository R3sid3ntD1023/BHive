#pragma once

#include "DragDropFactory.h"

namespace BHive
{
	// class to handle drag drop of audio assets
	class DragDropAudio : public DragDropFactory
	{
	public:
		virtual bool can_create(const rttr::type &type) override;

		virtual void post_create(const Ref<Asset> &asset, Ref<GameObject> &object) override;
	};

	// handles drag drop of skeletal meshes
	class DragDropSkeletalMesh : public DragDropFactory
	{
	public:
		virtual bool can_create(const rttr::type &type) override;

		virtual void post_create(const Ref<Asset> &asset, Ref<GameObject> &object) override;
	};

	// handles drag drop of static meshes
	class DragDropStaticMesh : public DragDropFactory
	{
	public:
		virtual bool can_create(const rttr::type &type) override;

		virtual void post_create(const Ref<Asset> &asset, Ref<GameObject> &entity) override;
	};

	// handles drag drop of prefabs
	struct DragDropPrefab : public DragDropFactory
	{
		virtual bool can_create(const rttr::type &type) override;

		virtual void post_create(const Ref<Asset> &asset, Ref<GameObject> &object) override;
	};
} // namespace BHive