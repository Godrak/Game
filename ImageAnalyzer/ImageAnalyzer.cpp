#include "ImageAnalyzer.h"
#include "LineDrawer.h"

using namespace ImageAnalyzer;
using namespace std;
using namespace LineDrawer;
using namespace LinAlgExtended;

Shape CountShapes(const vector<Shape> &shapes, int &count) {
    vector<int> results(SHAPE_DESCRIPTORS.size());
    for (const auto &shape: shapes) {
        if (shape != UNKNOWN) {
            results[shape]++;
        }
    }

    int maxIndex = -1;
    count = 0;
    for (int i = 0; i < results.size(); ++i) {
        if (results[i] > count) {
            count = results[i];
            maxIndex = i;
        }
    }

    return Shape(maxIndex);

}


Shape AnalyzeImageLines(ImageLines imageLines, float &matchingRotation) {
    imageLines.Normalize();
    auto translation = CreateTranslationMatrix({-0.5f, -0.5f});
    vector<vector<float>> outputs;
    vector<float> rotations;
    float t = 0.f;
    int iterations = 0;

    while (t < 1) {
        ImageLines copy = imageLines;
        auto rotation = CreateRotationMatrix(t);
        copy.Transform(mul<float, 3, 3>(rotation, translation));
        copy.Normalize();
        GrayScaleImage image = DrawLines(copy);
        outputs.push_back(NETWORK.Calculate(image.Serialize()));
        rotations.push_back(t);
        t += 1 / 36.f;
        iterations++;
    }

//    cout << endl;
//    for (const auto &out: outputs) {
//        cout << endl;
//        for (auto f :out)
//            cout << (int) (f * 100) << '\t';
//    }


    int maxIndex = -1;
    float maxShapeValue = -1;
    for (int row = 0; row < outputs.size(); ++row) {
        for (int shape = 0; shape < outputs[row].size(); ++shape) {
            if (outputs[row][shape] > maxShapeValue) {
                maxShapeValue = outputs[row][shape];
                matchingRotation = rotations[row];
                maxIndex = shape;
            }
        }
    }

    Shape shape = UNKNOWN;
    if (maxShapeValue > 0.8f) {
        shape = Shape(maxIndex);
    }

    return shape;
}


Shape AnalyzeImageLines(ImageLines imageLines) {
    float rotation;
    return AnalyzeImageLines(imageLines, rotation);
};

Line::Line(const float2 &startPoint, const float2 &endPoint) {
    start = float3{startPoint.x, startPoint.y, 1};
    end = float3{endPoint.x, endPoint.y, 1};
}

Line::Line(const float3 &start, const float3 &end) : start(start), end(end) {}


ShapeNode ImageAnalyzer::Analyze(ImageLines imageLines) {
    ShapeNode node;
    float matchingRotation;
    imageLines.Normalize();
    node.shape = AnalyzeImageLines(imageLines, matchingRotation);

    if (DEBUG_IMAGE_SAVE) {
        DrawLines(imageLines).SaveToFile("debug/im" + to_string(RANDOMIZER.ratio()));
    }
    cout << endl;
    cout << ShapeToString(node.shape) << "  " << "rotation: " << matchingRotation << endl;

    if (node.shape != UNKNOWN) {
        auto *shapeDescriptor = GetShapeDescriptor(node.shape);
        float t = 0;
        vector<Shape> pattern;
        while (t < 1) {
            ImageLines copy = imageLines;

            float2 point = shapeDescriptor->GetPoint(t);
            point = MovePoint(point, float2{-0.5f, -0.5f});
            point = RotatePoint(point, matchingRotation);
            point = MovePoint(point, float2{0.5f, 0.5f});

            copy.Clip(point - float2{0.1f, 0.1f}, point + float2{0.1f, 0.1f});

            pattern.push_back(AnalyzeImageLines(copy));
            if (!copy.Empty() && DEBUG_IMAGE_SAVE) {
                copy.Normalize();
                DrawLines(copy).SaveToFile("debug/pat" + to_string(RANDOMIZER.ratio()));
            }
            t += 1 / 36.f;
        }

        int count = 0;
        Shape patternShape = CountShapes(pattern, count);
        cout << ShapeToString(patternShape) << " " << count << endl;
        if (count > 6) {
            node.shapePattern = patternShape;
        }

        for (float3 poi : shapeDescriptor->GetPointsOfInterest()) {
            float2 middle{poi.x + poi.z / 2, poi.y + poi.z / 2};
            middle = MovePoint(middle, float2{-0.5f, -0.5f});
            middle = RotatePoint(middle, matchingRotation);
            middle = MovePoint(middle, float2{0.5f, 0.5f});

            ImageLines copy = imageLines;
            copy.Clip(middle - float2{poi.z / 2, poi.z / 2}, middle + float2{poi.z / 2, poi.z / 2});
            node.childNodes.push_back(Analyze(copy));
        }
    }

    return node;
}
