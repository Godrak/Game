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


linalg::aliases::float2 LinAlgExtended::RotatePoint(linalg::aliases::float2 point, float rotation, float2 middle) {
    auto translation = CreateTranslationMatrix(-middle);
    auto rotationMatrix = CreateRotationMatrix(rotation);
    auto translationBack = CreateTranslationMatrix(middle);
    float3 result = MatrixMul(MatrixMul(MatrixMul(translationBack, rotationMatrix), translation),
                              float3(point.x, point.y, 1));
    return {result.x, result.y};
}

linalg::aliases::float3x3 LinAlgExtended::CreateRotationMatrix(float parameter, linalg::aliases::float2 middle) {
    auto translation = CreateTranslationMatrix(-middle);
    auto rotationMatrix = CreateRotationMatrix(parameter);
    auto translationBack = CreateTranslationMatrix(middle);
    return MatrixMul(MatrixMul(translationBack, rotationMatrix), translation);
}

linalg::aliases::float3 LinAlgExtended::MatrixMul(linalg::aliases::float3x3 left, linalg::aliases::float3 vector) {
    return mul<float,3>(left,vector);
}

linalg::aliases::float3x3 LinAlgExtended::MatrixMul(linalg::aliases::float3x3 left, linalg::aliases::float3x3 right) {
    return mul<float,3,3>(left,right);
}
