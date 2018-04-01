#include "LinAlgExtended.h"

linalg::aliases::float3x3 LinAlgExtended::CreateTranslationMatrix(linalg::aliases::float2 vector) {
    float3x3 matrix{
            {1, 0, vector.x},
            {0, 1, vector.y},
            {0, 0, 1}
    };

    return transpose(matrix);
}

linalg::aliases::float3x3 LinAlgExtended::CreateScalingMatrix(linalg::aliases::float2 vector) {
    float3x3 matrix{
            {vector.x, 0,        0},
            {0,        vector.y, 0},
            {0,        0,        1},
    };

    return transpose(matrix);
}

linalg::aliases::float3x3 LinAlgExtended::CreateRotationMatrix(float parameter) {
    float radians = parameter * 2 * 3.14f;
    float3x3 matrix{
            {std::cos(radians), -std::sin(radians), 0},
            {std::sin(radians), std::cos(radians),  0},
            {0,                 0,                  1}
    };

    return transpose(matrix);
}

linalg::aliases::float2 LinAlgExtended::MovePoint(float2 point, linalg::aliases::float2 vector) {
    auto matrix = CreateTranslationMatrix(vector);
    float3 result = mul<float, 3>(matrix, float3{point.x, point.y, 1});
    return {result.x, result.y};

}

linalg::aliases::float2 LinAlgExtended::RotatePoint(linalg::aliases::float2 point, linalg::aliases::float3x3 matrix) {
    auto result = mul<float, 3>(matrix, float3{point.x, point.y, 1});
    return {result.x, result.y};
}
