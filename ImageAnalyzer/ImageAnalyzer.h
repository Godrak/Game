#pragma once

#include <vector>
#include <memory>
#include "LinAlgExtended.h"
#include "NeuralNetwork.h"
#include "Randomizer.h"

namespace ImageAnalyzer {
    using namespace std;
    using namespace LinAlgExtended;

    struct ShapeIndex {
        int value;

        ShapeIndex() {
            value = -1;
        }

        explicit ShapeIndex(int val) {
            value = val;
        }

        bool operator==(const ShapeIndex &rhs) const {
            return value == rhs.value;
        }

        bool operator!=(const ShapeIndex &rhs) const {
            return !(rhs == *this);
        }

        bool operator<(const ShapeIndex &rhs) const {
            return value < rhs.value;
        }

        bool operator>(const ShapeIndex &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const ShapeIndex &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const ShapeIndex &rhs) const {
            return !(*this < rhs);
        }
    };

    static const ShapeIndex UNKNOWN_SHAPE{};

    struct ShapeNode {
        ShapeIndex shape{};
        ShapeIndex shapePattern{};
        vector<ShapeNode> childNodes{};
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

    class ShapeDescriptor {
    protected:
        /// linear interpolation between two given points, to expected to be in range 0-1
        /// \param start
        /// \param end
        /// \param t
        /// \return
        float2 Interpolate(float2 start, float2 end, float t) const {
            return start + t * (end - start);
        }

        /// Normalizes param t to range 0 to 1
        /// \param t
        void NormalizeParam(float &t) const {
            t = abs(t - (int) t);
        }

    public:
        virtual string GetName() const = 0;

        /// Method used to generate training data.
        /// \param last_t param of last requested point.
        /// \param t tells which point from start to end of picture should be returned. Is in range from 0 to 1, bt it is recommended to normalize it first using NormalizeParam function.
        /// \param point coordinates of the point represented by t. Has to be in range (0-1,0-1).
        /// \return True if there is a line between last requested point and this point, otherwise false.
        virtual bool GetPoint(float last_t, float t, float2 &point) = 0;

        /// Method to inspect lines of shape, searching for composition, thread safety required
        /// \param t tells which point from start to end of picture should be returned. Is in range from 0 to 1, bt it is recommended to normalize it first using NormalizeParam function.
        /// \return point represented by t.
        virtual float2 GetPoint(float t) = 0;

        /// Method to find areas where "inside" shapes might be located. Currently only square shaped locations are supported.
        /// \return vector of locations in form (leftCornerX, leftCornerY, Size).
        virtual vector<float3> GetPointsOfInterest() const {
            return vector<float3>{};
        };
    };

    ShapeNode Analyze(ImageLines imageLines);

    void RegisterShapeDescriptor(ShapeIndex index, std::unique_ptr<ShapeDescriptor> shapeDescriptor);

    void LoadNetwork(const string &path);

    string GetNameByIndex(ShapeIndex index);

};