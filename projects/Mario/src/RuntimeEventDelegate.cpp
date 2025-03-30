#include "RuntimeEventDelegate.h"
#include "objects/GameObject.h"

namespace BHive
{
	RuntimeEventDelegate::RuntimeEventDelegate(const RuntimeEventDelegate &other)
	{
		SetCaller(other.Object);
	}

	void RuntimeEventDelegate::SetCaller(GameObject *caller)
	{
		Object = caller;
		if (Object)
			Object->OnDestroyedEvent.bind(this, &RuntimeEventDelegate::OnObjectDestroyed);
	}

	RuntimeEventDelegate &RuntimeEventDelegate::operator=(const RuntimeEventDelegate &rhs)
	{
		SetCaller(rhs.Object);
		return *this;
	}

	void RuntimeEventDelegate::OnObjectDestroyed(GameObject *)
	{
		Object = nullptr;
	}

} // namespace BHive
