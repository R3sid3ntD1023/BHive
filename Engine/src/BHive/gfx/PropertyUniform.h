#pragma once

#include "core/Core.h"
#include "gfx/Shader.h"

namespace BHive
{
	struct FUniformProperty
	{
		virtual const std::string &GetName() const = 0;

		virtual void Set(const std::any &v) = 0;

		virtual void Send(Ref<Shader> &shader) = 0;
	};

	template <typename T>
	struct TUniformProperty : public FUniformProperty
	{
		TUniformProperty(const std::string &name)
			: mName(name)
		{
		}

		const std::string &GetName() const { return mName; }

		virtual void Set(const std::any &v) override { mValue = std::any_cast<T>(v); }

		void Send(Ref<Shader> &shader) override { shader->SetUniform(mName.data(), mValue); }

	private:
		T mValue;
		std::string mName;
	};

	FUniformProperty *CreateProperty(const std::string &name, int32_t type);
} // namespace BHive