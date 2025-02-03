#pragma once

#include <future>
#include "Threading.h"

namespace BHive
{
    template<typename T>
    class SafePromise : public IJob
    {
        std::promise<T> mPromise;
        std::future<T> mFuture;
        ThreadFunction mFunc;

    public:
        SafePromise()
            :mFuture(mPromise.get_future()),
            mFunc([](){})
        {}

        SafePromise(ThreadFunction function)
            :mFuture(mPromise.get_future()),
            mFunc(function)
        {}

        void SetValue(T value)
        {
            mPromise.set_value(value);
        }

        std::future_status wait_for(float time)
        {
            return mFuture.wait_for(std::chrono::duration<float>(time));
        }

        bool IsReady()
        {
            return mFuture.wait_for(std::chrono::duration<float>(0.f)) == std::future_status::ready;
        }

        bool IsDone() override
        {
            return IsReady();
        }

        ThreadFunction GetDispatchedFunction() override
        {
            return mFunc;
        }

        T Get()
        {
            return mFuture.get();
        }
    };
} // namespace BHive
