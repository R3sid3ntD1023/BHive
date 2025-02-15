#pragma once

#include "core/EventDelegateHandle.h"
#include "core/Traits.hpp"
#include <functional>
#include <memory>

namespace BHive
{
	namespace utils
	{
		template <typename T, typename... Types>
		inline auto bind_object(T *object, void (T::*func)(Types...))
		{
			using Args = traits::has_args<Types...>;
			return [object, func](Types... args) { (object->*func)(args...); };
		}

		template <typename R, typename T, typename... Types>
		inline auto bind_object(T *object, R (T::*func)(Types...))
		{
			using Args = traits::has_args<Types...>;
			return [object, func](Types... args) { return (object->*func)(args...); };
		}

		template <typename R, typename T>
		inline auto bind_object(T *object, R (T::*func)())
		{
			return std::bind(func, object);
		}
	} // namespace utils

	struct IDelegateInstance
	{
		virtual ~IDelegateInstance() = default;

		virtual EventDelegateHandle GetHandle() const = 0;
	};

	template <typename TRet, typename... TArgs>
	struct EventDelegateBase : IDelegateInstance
	{
		using ReturnType = TRet;

		using ArgTypes = traits::has_args<TArgs...>;

		using Function = std::function<TRet(TArgs...)>;

		EventDelegateBase() = default;
		virtual ~EventDelegateBase() = default;

		EventDelegateBase(const EventDelegateBase &other)
			: mCallback(other.mCallback),
			  mHandle(other.mHandle)
		{
		}

		EventDelegateBase(EventDelegateBase &&other)
			: mCallback(std::move(other.mCallback)),
			  mHandle(std::move(other.mHandle))
		{
		}

		template <typename T>
		EventDelegateHandle bind(T *instance, ReturnType (T::*func)(TArgs...))
		{
			mCallback = utils::bind_object<ReturnType, T, TArgs...>(instance, func);
			mHandle.GenerateNewID();
			return GetHandle();
		}

		template <typename T>
		EventDelegateHandle bind(std::shared_ptr<T> instance, ReturnType (T::*func)(TArgs...))
		{
			mCallback = utils::bind_object<ReturnType, T, TArgs...>(instance.get(), func);
			mHandle.GenerateNewID();
			return GetHandle();
		}

		EventDelegateHandle bind(ReturnType (*func)(TArgs...))
		{
			mCallback = func;
			mHandle.GenerateNewID();
			return GetHandle();
		}

		template <typename TFunction>
		EventDelegateHandle bind(TFunction &&func)
		{
			mCallback = std::move(func);
			mHandle.GenerateNewID();
			return GetHandle();
		}

		EventDelegateBase &operator=(const EventDelegateBase &rhs)
		{

			mCallback = rhs.mCallback;
			mHandle = rhs.mHandle;
			return *this;
		}

		EventDelegateBase &operator=(EventDelegateBase &&rhs)
		{

			mCallback = std::move(rhs.mCallback);
			mHandle = std::move(rhs.mHandle);
			return *this;
		}

		void unbind(const EventDelegateHandle &handle)
		{
			if (handle == mHandle)
				mHandle.reset();
		}

		bool is_bound() const { return (bool)mHandle; }

		bool is_bound(const EventDelegateHandle &handle) const { return handle == GetHandle(); }

		EventDelegateHandle GetHandle() const { return mHandle; }

		operator bool() const { return (bool)mHandle; }

		Function get() const { return mCallback; }

		Function mCallback;

		EventDelegateHandle mHandle;
	};

	template <typename TRet, typename... TArgs>
	struct RetEventDelegate : public EventDelegateBase<TRet, TArgs...>
	{

		TRet invoke(TArgs... args) const
		{
			if (this->mCallback)
				return this->mCallback(args...);

			return TRet();
		}

		TRet operator()(TArgs... args) const { return invoke(args...); }

		TRet invoke(TArgs... args)
		{
			if (this->mCallback)
				return this->mCallback(args...);

			return TRet();
		}

		TRet operator()(TArgs... args) { return invoke(args...); }
	};

	template <typename... TArgs>
	struct EventDelegate : public EventDelegateBase<void, TArgs...>
	{
		EventDelegate() = default;

		void invoke(TArgs... args)
		{
			if (this->mCallback)
				this->mCallback(args...);
		}

		void operator()(TArgs... args) { invoke(args...); }
	};
} // namespace BHive

#define DECLARE_RET_EVENT(name, ...)                                   \
	struct name##Event : public ::BHive::RetEventDelegate<__VA_ARGS__> \
	{                                                                  \
	};
#define DECLARE_EVENT(name, ...)                                    \
	struct name##Event : public ::BHive::EventDelegate<__VA_ARGS__> \
	{                                                               \
		name##Event() = default;                                    \
	};
