#pragma once

#include "../LinAlg/linalg.h"

namespace LinAlgExtended {

    using namespace linalg;
    using namespace linalg::aliases;

    float3x3 CreateTranslationMatrix(float2 vector);

    float3x3 CreateScalingMatrix(float2 vector);

    float3x3 CreateRotationMatrix(float parameter);

    float2 MovePoint(float2 point,float2 vector);

    float2 RotatePoint(float2 point, float rotation);

}