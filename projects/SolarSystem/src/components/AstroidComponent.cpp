#include "AstroidComponent.h"
#include "CelestrialBody.h"
#include "mesh/indirect_mesh/IndirectMesh.h"
#include "renderer/RenderPipeline.h"
#include "renderer/ShaderInstance.inl"

BEGIN_NAMESPACE(BHive)

float GetRandomDisplacement(float offset)
{
	return (rand() % (int)(2 * offset * 100)) / 100.f - offset;
}

void AstroidComponent::Update(float dt)
{
	if (auto pipeline = BHive::GetRenderPipelineManager().GetCurrentPipeline())
	{
		pipeline->SubmitMesh(mIndirectMesh, GetOwner()->GetWorldTransform(), mShaderInstance, Instances, mMatrices.data());
	}
}

void AstroidComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP(Instances), MAKE_NVP(Seed), MAKE_NVP(Radius), MAKE_NVP(Offset));
	MeshComponent::Save(ar);
}

void AstroidComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP(Instances), MAKE_NVP(Seed), MAKE_NVP(Radius), MAKE_NVP(Offset));
	MeshComponent::Load(ar);

	CalculateMatrices();
}

void AstroidComponent::InitIndirectMesh(const Ref<BHive::BaseMesh> &renderable, Ref<BHive::IndirectRenderable> &indirect)
{
	indirect->Init(renderable, Instances);
}

void AstroidComponent::CalculateMatrices()
{
	mMatrices.resize(Instances);

	const float &radius = Radius;
	const float &offset = Offset;
	srand(Seed);
	for (uint32_t i = 0; i < Instances; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		float angle = (float)i / (float)Instances * 360.f;
		float displacement = GetRandomDisplacement(offset);
		float x = sinf(angle) * radius + displacement;
		displacement = GetRandomDisplacement(offset);
		float y = displacement * 0.4f;
		displacement = GetRandomDisplacement(offset);
		float z = cosf(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		float scale = (rand() % 20) / 100.f + 0.05f;
		model = glm::scale(model, glm::vec3(scale) * 10.f);

		float rot = (float)(rand() % 360);
		model = glm::rotate(model, rot, glm::vec3(0.4f, 0.6f, 0.8f));

		mMatrices[i] = model;
	}
}

REFLECT(AstroidComponent)
{
	BEGIN_REFLECT(AstroidComponent)
	REFLECT_CONSTRUCTOR() COMPONENT_IMPL();
}

END_NAMESPACE