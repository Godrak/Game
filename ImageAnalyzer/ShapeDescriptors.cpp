#include "ShapeDescriptors.h"

namespace ShapeDescriptors {

    std::string ShapeToString(Shape shape) {
        switch (shape) {
            case RECTANGLE:
                return "RECTANGLE";
            case CIRCLE:
                return "CIRCLE";
            case TRIANGLE:
                return "TRIANGLE";
            case UNKNOWN:
                return "UNKNOWN";
            case WAVY:
                return "WAVY";
            case WATER:
                return "WATER";
            case LIGHTNING:
                return "LIGHTNING";
        }
    }

    const ShapeDescriptor *GetShapeDescriptor(Shape shape) {
        switch (shape) {
            case UNKNOWN:
                return nullptr;
            case RECTANGLE:
                return &RECTANGLE_INSTANCE;
            case CIRCLE:
                return &CIRCLE_INSTANCE;
            case TRIANGLE:
                return &TRIANGLE_INSTANCE;
            case WAVY:
                return &WAVY_INSTANCE;
            case WATER:
                return &WATER_INSTANCE;
            case LIGHTNING:
                return &LIGHTNING_INSTANCE;
        }
    }

    Shape CircleDescriptor::GetName() const {
        return CIRCLE;
    }

    float2 CircleDescriptor::GetPoint(float t) const {
        NormalizeParam(t);
        float radians = t * 2 * (float) M_PI;
        return {center.x + radius * std::cos(radians), center.y + radius * std::sin(radians)};

    }

    vector<float3> CircleDescriptor::GetPointsOfInterest() const {
        return vector<float3>{float3{center.x - 0.3f, center.y - 0.3f, 0.6}};
    }

    Shape RectangleDescriptor::GetName() const {
        return RECTANGLE;
    }

    float2 RectangleDescriptor::GetPoint(float t) const {
        NormalizeParam(t);

        if (t <= 1 / 4.f) {
            return Interpolate(topLeft, topRight, t * 4);
        } else if (t <= 1 / 2.f) {
            return Interpolate(topRight, bottomRight, (t - 1 / 4.f) * 4);
        } else if (t <= 3 / 4.f) {
            return Interpolate(bottomRight, bottomLeft, (t - 1 / 2.f) * 4);
        } else {
            return Interpolate(bottomLeft, topLeft, (t - 3 / 4.f) * 4);
        }
    }

    vector<float3> RectangleDescriptor::GetPointsOfInterest() const {
        return vector<float3>{float3(topLeft.x * 4, topLeft.y * 4, 0.6f)};
    }

    Shape TriangleDescriptor::GetName() const {
        return TRIANGLE;
    }

    float2 TriangleDescriptor::GetPoint(float t) const {
        NormalizeParam(t);

        if (t <= 1 / 3.f) {
            return Interpolate(top, left, t * 3);
        } else if (t <= 2 / 3.f) {
            return Interpolate(left, right, (t - 1 / 3.f) * 3);
        } else {
            return Interpolate(right, top, (t - 2 / 3.f) * 3);
        }
    }

    vector<float3> TriangleDescriptor::GetPointsOfInterest() const {
        return vector<float3>{float3{0.2, 0.3, 0.6}};
    }

    Shape WavyDescriptor::GetName() const {
        return WAVY;
    }

    float2 WavyDescriptor::GetPoint(float t) const {
        NormalizeParam(t);

        return {t, (0.5f - std::sin(t * 2 * (float) M_PI) / 4.f)};
    }

    Shape LightningDescriptor::GetName() const {
        return LIGHTNING;
    }

    float2 LightningDescriptor::GetPoint(float t) const {
        NormalizeParam(t);

        if (t <= 1 / 3.f) {
            return Interpolate(top, right, t * 3);
        } else if (t <= 2 / 3.f) {
            return Interpolate(right, left, (t - 1 / 3.f) * 3);
        } else {
            return Interpolate(left, bottom, (t - 2 / 3.f) * 3);
        }
    }

    Shape WaterDropDescriptor::GetName() const {
        return WATER;
    }

    float2 WaterDropDescriptor::GetPoint(float t) const {
        NormalizeParam(t);
        return float2{
                0.5f + 0.5f * (1 - cos(2 * (float) M_PI * t)) * sin(2 * (float) M_PI * t) / 2.f,
                0.5f - cos(2 * (float) M_PI * t) / 2.f
        };
    }

    vector<float3> WaterDropDescriptor::GetPointsOfInterest() const {
        return vector<float3>{
                float3{0.2, 0.4, 0.6}
        };
    }
};