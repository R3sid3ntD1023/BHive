#pragma once

#include <core/Core.h>
#include <core/serialization/Serialization.h>
#include <core/UUID.h>

struct CelestrialBody;

namespace BHive
{
	class Shader;
	class VertexBuffer;
	class StaticMesh;
} // namespace BHive

class Universe
{
public:
	using ObjectList = std::unordered_map<BHive::UUID, Ref<CelestrialBody>>;

public:
	Universe();

	template <typename T>
	Ref<CelestrialBody> AddBody()
	{
		auto body = CreateRef<T>(this);
		AddBody(body);
		return body;
	}

	void AddBody(const Ref<CelestrialBody> &body);

	void Begin();

	void Update(float dt);

	void Save(cereal::JSONOutputArchive &ar) const;
	void Load(cereal::JSONInputArchive &ar);

	Ref<CelestrialBody> GetBody(const BHive::UUID &id) const;

	const ObjectList &GetObjects() const { return mBodies; }
	ObjectList &GetObjects() { return mBodies; }

private:
	ObjectList mBodies;
};
