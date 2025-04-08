#pragma once

#include "world/World.h"
#include <core/Core.h>
#include <core/serialization/Serialization.h>
#include <core/UUID.h>

BEGIN_NAMESPACE(BHive)

struct CelestrialBody;

class Shader;
class VertexBuffer;
class StaticMesh;

class Universe : public World
{
public:
public:
	Universe();
};

END_NAMESPACE