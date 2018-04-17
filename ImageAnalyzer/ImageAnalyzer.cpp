#include "ImageAnalyzer.h"
#include "LineDrawer.h"

using namespace ImageAnalyzer;
using namespace std;
using namespace LineDrawer;
using namespace LinAlgExtended;

namespace { //private
    const bool DEBUG_IMAGE_SAVE = true;
    const int DEBUG_OUTPUT = 1;
    const int IMAGE_SIDE_SIZE = 32;
    const float LINE_DRAWING_STEP_SIZE = 0.5f;

    Randomizer randomizer{};
    std::map<ShapeIndex, std::unique_ptr<ShapeDescriptor>> shapeDescriptors{};
    NeuralNetwork neuralNetwork{};
}

ShapeIndex CountShapes(const vector<ShapeIndex> &shapes, int &count) {
    std::map<ShapeIndex, int> results{};
    for (const auto &shape: shapes) {
        if (shape != UNKNOWN_SHAPE) {
            results.operator[](shape)++;
        }
    }

    ShapeIndex maxIndex = UNKNOWN_SHAPE;
    count = 0;
    auto it = results.begin();
    while (it != results.end()) {
        if (it->second > count) {
            count = it->second;
            maxIndex = it->first;
        }
        it++;
    }

    return maxIndex;
}


ShapeIndex AnalyzeImageLines(ImageLines imageLines, float &matchingRotation) {
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
        GrayScaleImage image = DrawLines(copy, IMAGE_SIDE_SIZE, LINE_DRAWING_STEP_SIZE);
//        RandomizeData(image, randomizer);
        auto input = image.Serialize();
        outputs.push_back(neuralNetwork.Calculate(input));
        rotations.push_back(t);
        t += 1 / 36.f;
        iterations++;
    }

    if (DEBUG_OUTPUT > 1) {
        cout << endl;
        for (const auto &out: outputs) {
            cout << endl;
            for (auto f :out)
                cout << (int) (f * 100) << '\t';
        }
    }

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

    int shape = -1;
    if (maxShapeValue > 0.8f) {
        shape = maxIndex;
    }

    return ShapeIndex(shape);
}


ShapeIndex AnalyzeImageLines(ImageLines imageLines) {
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
        DrawLines(imageLines, IMAGE_SIDE_SIZE, LINE_DRAWING_STEP_SIZE).SaveToFile("debug/im" + to_string(randomizer.ratio()));
    }

    if (DEBUG_OUTPUT > 0)
        cout << endl << GetNameByIndex(node.shape) << "  " << "rotation: " << matchingRotation << endl;

    auto shapeDesc = shapeDescriptors.find(node.shape);
    if (shapeDesc != shapeDescriptors.end()) {
        auto *shapeDescriptor = shapeDesc->second.get();
        float t = 0;
        vector<ShapeIndex> pattern;
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
                DrawLines(copy, IMAGE_SIDE_SIZE, LINE_DRAWING_STEP_SIZE).SaveToFile("debug/pat" + to_string(randomizer.ratio()));
            }
            t += 1 / 36.f;
        }

        int count = 0;
        ShapeIndex patternShape = CountShapes(pattern, count);

        if (DEBUG_OUTPUT > 0)
            cout << GetNameByIndex(patternShape) << " " << count << endl;

        if (count > 3) {
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

string ImageAnalyzer::GetNameByIndex(ShapeIndex index) {
    if (index == UNKNOWN_SHAPE) {
        return "unknown";
    } else {
        auto shapeDesc = shapeDescriptors.find(index);
        if (shapeDesc == shapeDescriptors.end()) {
            cout << "ERROR: invalid shape index: " << index.value << endl;
            return "unknown";
        } else {
            return shapeDesc->second->GetName();
        }
    }
}

void ImageAnalyzer::LoadNetwork(const string &path) {
    neuralNetwork.Load(path.c_str());
}

void ImageAnalyzer::RegisterShapeDescriptor(ShapeIndex index, std::unique_ptr<ShapeDescriptor> shapeDescriptor) {
    if (shapeDescriptors.find(index) != shapeDescriptors.end()) {
        cout << "ERROR: shape with index: " << index.value << " already present" << endl;
    }

    if (index == UNKNOWN_SHAPE) {
        cout << "ERROR: attempt to register uninitialized index or index of UNKNOWN_SHAPE" << endl;
    }
    shapeDescriptors.emplace(index, move(shapeDescriptor));
}

