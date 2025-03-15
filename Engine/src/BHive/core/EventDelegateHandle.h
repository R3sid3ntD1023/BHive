#pragma once

#include "core/UUID.h"

namespace BHive
{

	struct EventDelegateHandle
	{
		enum class EGenerateNewHandle
		{
			GenerateNewHandle
		};

		EventDelegateHandle();
		EventDelegateHandle(EGenerateNewHandle);
		EventDelegateHandle(const EventDelegateHandle &) = default;

		void reset();

		bool operator==(const EventDelegateHandle &rhs) const;
		bool operator!=(const EventDelegateHandle &rhs) const;

		operator bool() const;

	private:
		void GenerateNewID();

		UUID mID = UUID::Null;

		template <typename TRet, typename... TArgs>
		friend struct EventDelegateBase;
	};
} // namespace BHive
