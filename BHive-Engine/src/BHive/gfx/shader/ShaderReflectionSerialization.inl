#include "ShaderReflection.h"

namespace BHive
{

	template <typename A>
	void SERIALIZE(A &ar, FUniform &u)
	{
		ar(u.Type, u.Size, u.Offset, u.Location, u.Stages);
	}

	template <typename A>
	void SERIALIZE(A &ar, FSampler &s)
	{
		ar(s.Semantic, s.Binding, s.Stages, s.ArraySize, s.Type);
	}

	template <typename A>
	void SERIALIZE(A &ar, FUniformBuffer &b)
	{
		ar(b.Semantic, b.Binding, b.Size, b.Stages, b.Members);
	}

	template <typename A>
	void SERIALIZE(A &ar, FStorageBuffer &b)
	{
		ar(b.Semantic, b.Binding, b.Size, b.Stages);
	}

	template <typename A>
	void SERIALIZE(A &ar, FPushConstantsRange &r)
	{
		ar(r.Size, r.Offset, r.Stages, r.Members);
	}

	template <typename A>
	void SERIALIZE(A &ar, FSetReflection &obj)
	{
		ar(obj.UniformBuffers, obj.StorageBuffers, obj.Samplers);
	}

	template <typename A>
	void SERIALIZE(A &ar, FShaderReflection &obj)
	{
		ar(obj.Sets, obj.Uniforms, obj.PushConstants);
	}

	template <typename A>
	void SERIALIZE(A &ar, FReflectedResource &r)
	{
		ar(r.Semantic, r.kind, r.binding, r.name, r.location, r.offset, r.size);
	}

} // namespace BHive