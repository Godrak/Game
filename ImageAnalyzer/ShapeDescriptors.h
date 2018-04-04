#pragma once

#include <vector>
#include "LinAlgExtended.h"
#include <math.h>

using namespace LinAlgExtended;
using namespace std;

namespace ShapeDescriptors {
    enum Shape {
        UNKNOWN = -1, RECTANGLE = 0, CIRCLE = 1, TRIANGLE = 2, WATER = 3, LIGHTNING = 4, WAVY = 5,
    };

    std::string ShapeToString(Shape shape);

    class ShapeDescriptor {
    protected:
        float2 Interpolate(float2 start, float2 end, float t) const {
            return start + t * (end - start);
        }

        void NormalizeParam(float &t) const {
            t = abs(t - (int) t);
        }

    public:
        virtual Shape GetName() const = 0;

        virtual float2 GetPoint(float t) const = 0;

        virtual vector<float3> GetPointsOfInterest() const {
            return vector<float3>{};
        };
    };

    class RectangleDescriptor : public ShapeDescriptor {
    private:
        float2 topLeft = {0.05f, 0.05f};
        float2 topRight = {0.95f, 0.05f};
        float2 bottomLeft = {0.05f, 0.95f};
        float2 bottomRight = {0.95f, 0.95f};
    public:
        Shape GetName() const override;

        float2 GetPoint(float t) const override;

        vector<float3> GetPointsOfInterest() const override;
    };

    class CircleDescriptor : public ShapeDescriptor {
    private:
        float2 center = {0.5f, 0.5f};
        float radius = 0.4f;

    public:
        Shape GetName() const override;

        float2 GetPoint(float t) const override;

        vector<float3> GetPointsOfInterest() const override;
    };

    class TriangleDescriptor : public ShapeDescriptor {
    private:
        float2 top = {0.5, 0.05};
        float2 left = {0.05, 0.95};
        float2 right = {0.95, 0.95};

    public:
        Shape GetName() const override;

        float2 GetPoint(float t) const override;

        vector<float3> GetPointsOfInterest() const override;
    };

    class WavyDescriptor : public ShapeDescriptor {

    public:
        Shape GetName() const override;

        float2 GetPoint(float t) const override;
    };

    class LightningDescriptor : public ShapeDescriptor {
    private:
        float2 top{0.7,0.05};
        float2 right{0.2,0.55};
        float2 left{0.8,0.45};
        float2 bottom{0.3,0.95};
    public:
        Shape GetName() const override;

        float2 GetPoint(float t) const override;
    };

    class WaterDropDescriptor : public ShapeDescriptor {
    public:
        Shape GetName() const override;

        float2 GetPoint(float t) const override;

        vector<float3> GetPointsOfInterest() const override;
    };

    static const RectangleDescriptor RECTANGLE_INSTANCE{};
    static const CircleDescriptor CIRCLE_INSTANCE{};
    static const TriangleDescriptor TRIANGLE_INSTANCE{};
    static const WaterDropDescriptor WATER_INSTANCE{};
    static const LightningDescriptor LIGHTNING_INSTANCE{};
    static const WavyDescriptor WAVY_INSTANCE{};

    static const vector<const ShapeDescriptor *> SHAPE_DESCRIPTORS{
            &RECTANGLE_INSTANCE,
            &CIRCLE_INSTANCE,
            &TRIANGLE_INSTANCE,
            &WATER_INSTANCE,
            &LIGHTNING_INSTANCE,
            &WAVY_INSTANCE,
    };

    const ShapeDescriptor *GetShapeDescriptor(Shape shape);

};
