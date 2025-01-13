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

} // namespace BHive
