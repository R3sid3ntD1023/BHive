#pragma once

#include "core/Core.h"
#include "math/Math.h"

namespace BHive
{

    template <typename T>
    struct TKeyFrame
    {
        T mValue;
        float mTimeStamp;
    };

    struct FrameData
    {
        std::vector<TKeyFrame<glm::vec3>> mPositions;
        std::vector<TKeyFrame<glm::quat>> mRotations;
        std::vector<TKeyFrame<glm::vec3>> mScales;
    };

    template <typename T>
    inline void Serialize(StreamWriter &ar, const TKeyFrame<T> &obj)
    {
        ar(obj.mValue, obj.mTimeStamp);
    }

    template <typename T>
    inline void Deserialize(StreamReader &ar, TKeyFrame<T> &obj)
    {
        ar(obj.mValue, obj.mTimeStamp);
    }

    inline void Serialize(StreamWriter &ar, const FrameData &obj)
    {
        ar(obj.mPositions, obj.mRotations, obj.mScales);
    }

    inline void Deserialize(StreamReader &ar, FrameData &obj)
    {
        ar(obj.mPositions, obj.mRotations, obj.mScales);
    }
} // namespace BHive
