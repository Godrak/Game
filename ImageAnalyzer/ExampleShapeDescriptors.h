#pragma once

#include "ImageAnalyzer.h"

using namespace ImageAnalyzer;

class SquareDescriptor : public ShapeDescriptor {
private:
    float2 topLeft = {0.0f, 0.0f};
    float2 topRight = {1, 0};
    float2 bottomLeft = {0, 1};
    float2 bottomRight = {1, 1};
public:
    string GetName() const override {
        return "square";
    }

    float2 GetPoint(float t) override {
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

    bool GetPoint(float last_t, float t, float2 &point) override {
        point = GetPoint(t);
        if (last_t > t) return false;
        return true;
    }

    std::vector<float3> GetPointsOfInterest() const override {
        return std::vector<float3>{float3(topLeft.x + 0.2f, topLeft.y + 0.2f, 0.6f)};
    }
};

class CircleDescriptor : public ShapeDescriptor {
private:
    float2 center = {0.5f, 0.5f};
    float radius = 0.5f;

public:
    string GetName() const override {
        return "circle";
    }

    float2 GetPoint(float t) override {
        NormalizeParam(t);
        float radians = t * 2 * (float) M_PI;
        return {center.x + radius * std::cos(radians), center.y + radius * std::sin(radians)};
    }

    bool GetPoint(float last_t, float t, float2 &point) override {
        point = GetPoint(t);
        if (last_t > t) return false;
        return true;
    }

    std::vector<float3> GetPointsOfInterest() const override {
        return std::vector<float3>{float3{center.x - 0.2f, center.y - 0.2f, 0.4}};
    }
};

class TriangleDescriptor : public ShapeDescriptor {
private:
    float2 top = {0.5, 0.0};
    float2 left = {0.0, 1};
    float2 right = {1, 1};

public:
    string GetName() const override {
        return "triangle";
    }

    float2 GetPoint(float t) override {
        NormalizeParam(t);

        if (t <= 1 / 3.f) {
            return Interpolate(top, left, t * 3);
        } else if (t <= 2 / 3.f) {
            return Interpolate(left, right, (t - 1 / 3.f) * 3);
        } else {
            return Interpolate(right, top, (t - 2 / 3.f) * 3);
        }
    }

    bool GetPoint(float last_t, float t, float2 &point) override {
        point = GetPoint(t);
        if (last_t > t) return false;
        return true;
    }

    std::vector<float3> GetPointsOfInterest() const override {
        return std::vector<float3>{float3{0.35, 0.55, 0.3}};
    }
};

class CrossDescriptor : public ShapeDescriptor {
    float2 top = {0.5, 0.0};
    float2 bottom = {0.5, 0.0};
    float2 left = {0.0, 0.5};
    float2 right = {1, 0.5};
public:
    string GetName() const override {
        return "cross";
    }

    float2 GetPoint(float t) override {
        NormalizeParam(t);

        if (t < 0.5f) {
            return Interpolate(top, bottom, t / 0.5f);
        } else {
            return Interpolate(left, right, (t - 0.5f) / 0.5f);
        }
    }

    bool GetPoint(float last_t, float t, float2 &point) override {
        NormalizeParam(last_t);
        NormalizeParam(t);

        point = GetPoint(t);
        if (last_t < 0.5f && t >= 0.5f || last_t > t) {
            return false;
        }
        return true;
    }

    vector<float3> GetPointsOfInterest() const override {
        return vector<float3>{
                float3(0.0, 0.0, 0.4f),
                float3(0.6, 0.0, 0.4f),
                float3(0.0, 0.6, 0.4f),
                float3(0.6, 0.6, 0.4f),
        };
    }
};

class FlashDescriptor : public ShapeDescriptor {
private:
    float2 top{0.7, 0.0};
    float2 right{0.0, 0.55};
    float2 left{0.8, 0.45};
    float2 bottom{0.3, 1};
public:
    string GetName() const override {
        return "flash";
    }

    float2 GetPoint(float t) override {
        NormalizeParam(t);

        if (t <= 1 / 3.f) {
            return Interpolate(top, right, t * 3);
        } else if (t <= 2 / 3.f) {
            return Interpolate(right, left, (t - 1 / 3.f) * 3);
        } else {
            return Interpolate(left, bottom, (t - 2 / 3.f) * 3);
        }
    }

    bool GetPoint(float last_t, float t, float2 &point) override {
        point = GetPoint(t);
        if (last_t > t) return false;
        return true;
    }
};

class WaterDropDescriptor : public ShapeDescriptor {
public:
    string GetName() const override {
        return "water";
    }

    float2 GetPoint(float t) override {
        NormalizeParam(t);
        return float2{
                0.5f + (1 - std::cos(2 * (float) M_PI * t)) * std::sin(2 * (float) M_PI * t) / 2.8f,
                0.5f - std::cos(2 * (float) M_PI * t) / 2.f
        };
    }

    bool GetPoint(float last_t, float t, float2 &point) override {
        point = GetPoint(t);
        if (last_t > t) return false;
        return true;
    }

    std::vector<float3> GetPointsOfInterest() const override {
        return std::vector<float3>{
                float3{0.3, 0.5, 0.3}
        };
    }
};
