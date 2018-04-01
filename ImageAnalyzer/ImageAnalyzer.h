#pragma once

#include <vector>
#include "LinAlgExtended.h"
#include "ShapeDescriptors.h"

namespace ImageAnalyzer {
    using namespace std;
    using namespace LinAlgExtended;
    using namespace ShapeDescriptors;

    struct ShapeNode {
        Shape shape;
        Shape shapePattern;
        vector<ShapeNode> childNodes{};

        ShapeNode() {
            shape = UNKNOWN;
            shapePattern = UNKNOWN;
        }

    };

    class Line {
    private:
        float3 start;
        float3 end;

        Line(const float3 &start, const float3 &end);

    public:
        float2 GetStart() const {
            return {start.x, start.y};
        }

        float2 GetEnd() const {
            return {end.x, end.y};
        }

        Line(const float2 &start, const float2 &end);

        void Transform(float3x3 matrix) {
            start = mul<float, 3>(matrix, start);
            end = mul<float, 3>(matrix, end);
        }
    };

    class ImageLines {
    private:
        vector<Line> lines{};

        float MinElem(float a, float b, float c) {
            if (a < b && a < c) return a;
            else if (b < c) return b;
            else return c;
        };

        float MaxElem(float a, float b, float c) {
            if (a > b && a > c) return a;
            else if (b > c) return b;
            else return c;
        }

        float4 MaxXYminXY() {
            if (lines.empty()) return {0, 0, 0, 0};
            float2 max = lines[0].GetStart();
            float2 min = lines[0].GetEnd();
            for (const auto &line : lines) {
                max.x = MaxElem(line.GetStart().x, line.GetEnd().x, max.x);
                min.x = MinElem(line.GetStart().x, line.GetEnd().x, min.x);
                max.y = MaxElem(line.GetStart().y, line.GetEnd().y, max.y);
                min.y = MinElem(line.GetStart().y, line.GetEnd().y, min.y);
            }
            return {max.x, max.y, min.x, min.y};
        };

        bool CheckBounds(const Line &line, float2 topLeft, float2 bottomRight) {
            return (line.GetStart().x >= topLeft.x && line.GetStart().x <= bottomRight.x &&
                    line.GetStart().y >= topLeft.y && line.GetStart().y <= bottomRight.y) ||
                   (line.GetEnd().x >= topLeft.x && line.GetEnd().x <= bottomRight.x &&
                    line.GetEnd().y >= topLeft.y && line.GetEnd().y <= bottomRight.y);
        };

    public:
        explicit ImageLines(const vector<Line> &lines) : lines(lines) {}

        ImageLines() = default;

        const vector<Line> &GetImageLines() const {
            return lines;
        }

        void Transform(float3x3 matrix) {
            for (auto &line : lines) {
                line.Transform(matrix);
            }
        }

        bool Empty() const {
            return lines.empty();
        }

        void Normalize() {
            auto maxmin = MaxXYminXY();

            auto size = abs(maxmin.x - maxmin.z);
            size = max(size, abs(maxmin.y - maxmin.w));

            auto translation = CreateTranslationMatrix(float2{-maxmin.z, -maxmin.w});
            auto scaling = CreateScalingMatrix(float2{1 / size, 1 / size});
            auto matrix = mul<float, 3, 3>(scaling, translation);

            Transform(matrix);
        };

        void Clip(float2 topLeft, float2 bottomRight) {
            vector<Line> result;
            for (const auto &line : lines) {
                if (CheckBounds(line, topLeft, bottomRight)) {
                    result.push_back(line);
                }
            }
            lines = result;
        }

        void Add(ImageLines imageLines) {
            for (const auto &line: imageLines.GetImageLines()) {
                lines.push_back(line);
            }
        }

    };


    ShapeNode Analyze(ImageLines imageLines);

};