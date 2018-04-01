#include "ImageAnalyzer.h"
#include "LineDrawer.h"

using namespace ImageAnalyzer;
using namespace std;
using namespace LineDrawer;
using namespace LinAlgExtended;

Shape AnalyzeImageLines(ImageLines imageLines, float3x3 &matchingRotation) {
    imageLines.Normalize();
    auto translation = CreateTranslationMatrix({-0.5f, -0.5f});
    vector<vector<float>> outputs;
    vector<float3x3> rotations;
    float t = 0.f;
    int iterations = 0;

    while (t < 1) {
        ImageLines copy = imageLines;
        auto rotation = CreateRotationMatrix(t);
        copy.Transform(mul<float, 3, 3>(rotation, translation));
        copy.Normalize();
        GrayScaleImage image = DrawLines(copy);
        outputs.push_back(NETWORK.Calculate(image.Serialize()));
        rotations.push_back(rotation);
        t += 1 / 36.f;
        iterations++;
    }

    cout << endl;
    for (const auto &out: outputs) {
        cout << endl;
        for (auto f :out)
            cout << (int) (f * 100) << '\t';
    }


    int maxIndex = -1;
    float maxShapeValue = -1;
    for (int i = 0; i < outputs[0].size(); ++i) {
        float sum = 0;
        for (auto &output : outputs) {
            sum += output[i];
        }
        if (sum > maxShapeValue) {
            maxShapeValue = sum;
            maxIndex = i;
        }
    }

    Shape shape = UNKNOWN;
    if (maxShapeValue > iterations / 3.f) {
        shape = Shape(maxIndex);
        int rotationRow = 0;
        float rotationMax = 0;
        for (int i = 0; i < outputs.size(); ++i) {
            if (outputs[i][maxIndex] > rotationMax) {
                rotationMax = outputs[i][maxIndex];
                rotationRow = i;
            }
        }
        matchingRotation = rotations[rotationRow];
    }

    return shape;
}


Shape AnalyzeImageLines(ImageLines imageLines) {
    float3x3 matrix;
    return AnalyzeImageLines(imageLines, matrix);
};

Line::Line(const float2 &startPoint, const float2 &endPoint) {
    start = float3{startPoint.x, startPoint.y, 1};
    end = float3{endPoint.x, endPoint.y, 1};
}

Line::Line(const float3 &start, const float3 &end) : start(start), end(end) {}


ShapeNode ImageAnalyzer::Analyze(ImageLines imageLines) {
    ShapeNode node;
    float3x3 matchingRotation;
    imageLines.Normalize();
    node.shape = AnalyzeImageLines(imageLines, matchingRotation);

    if (DEBUG_IMAGE_SAVE) {
        DrawLines(imageLines).SaveToFile("debug/im" + to_string(RANDOMIZER.ratio()));
    }
    cout << ShapeToString(node.shape) << endl;
    return node;

    if (node.shape != UNKNOWN) {
        auto *shapeDescriptor = GetShapeDescriptor(node.shape);
        float t = 0;
        vector<Shape> pattern;
        while (t < 1) {
            ImageLines copy = imageLines;
            float2 point = shapeDescriptor->GetPoint(t);
//            point = MovePoint(point, float2{-0.5f, -0.5f});
//            point = RotatePoint(point, (matchingRotation));
//            copy.Transform(inverse(matchingRotation));
            copy.Clip(point - float2{0.2f, 0.2f}, point + float2{0.2f, 0.2f});

            pattern.push_back(AnalyzeImageLines(copy));
            if (!copy.Empty()) {
                copy.Normalize();
                DrawLines(copy).SaveToFile("debug/pat" + to_string(RANDOMIZER.ratio()));
            }
            t += 1 / 12.f;
        }
        for (Shape s : pattern) {
            cout << ShapeToString(s) << " ";
        }
        cout << endl;
    }

    return node;
}
