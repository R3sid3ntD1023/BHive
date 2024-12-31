#pragma once

#include <stdint.h>
#include <memory>

template <typename T>
inline T *Cast(void *pointer)
{
	return static_cast<T *>(pointer);
}

template <typename T>
inline T Cast(uintptr_t pointer)
{
	return static_cast<T>(pointer);
}

template <typename T, typename U>
inline const T *Cast(const U *ptr)
{
	return dynamic_cast<const T *>(ptr);
}

template <typename T, typename U>
inline T *Cast(U *ptr)
{
	return dynamic_cast<T *>(ptr);
}

template <typename T, typename U>
inline std::shared_ptr<T> Cast(const std::shared_ptr<U> &ptr)
{
	return std::dynamic_pointer_cast<T>(ptr);
}
