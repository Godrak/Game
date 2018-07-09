#include "ImageAnalyzer.h"
#include "LineDrawer.h"

using namespace ImageAnalyzer;
using namespace std;
using namespace LineDrawer;
using namespace LinAlgExtended;

namespace { //private
    Randomizer randomizer{};
    std::map<ShapeIndex, std::unique_ptr<ShapeDescriptor>> shapeDescriptors{};
    NeuralNetwork neuralNetwork{};
}

namespace ImageAnalyzer {
    bool DEBUG_IMAGE_SAVE = false;
    bool COMPOSED_SHAPES_ENABLED = true;
    int COMPOSITION_SAMPLES_COUNT = 16;
    int COMPOSITION_SAMPLES_LIMIT = 5;
    float COMPOSITION_WINDOW_SIZE = 0.1f;
    bool EMBEDDED_SHAPES_ENABLED = true;
    bool ROTATIONS_ENABLED = true;
    int ROTATION_SAMPLES_COUNT = 13;
    int DEBUG_OUTPUT = 1;
    int IMAGE_SIDE_SIZE = 32;
    float LINE_DRAWING_STEP_SIZE = 0.5f;
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
    vector<vector<float>> outputs;
    vector<float> rotations;
    float t = 0.f;
    float tStep = ROTATIONS_ENABLED ? 1/(float)ROTATION_SAMPLES_COUNT : 1.f;
    int iterations = 0;

    while (t < 1) {
        ImageLines copy = imageLines;
        copy.Transform(CreateRotationMatrix(t, float2(0.5f, 0.5f)));
        copy.Normalize();
        GrayScaleImage image = DrawLines(copy, IMAGE_SIDE_SIZE, LINE_DRAWING_STEP_SIZE);
        RandomizeData(image, randomizer);
        auto input = image.Serialize();
        outputs.push_back(neuralNetwork.Calculate(input));
        rotations.push_back(t);
        t += tStep;
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
    if (maxShapeValue > 0.6f) {
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
        DrawLines(imageLines, IMAGE_SIDE_SIZE, LINE_DRAWING_STEP_SIZE).SaveToFile(
                "debug/im" + to_string(randomizer.ratio()));
    }

    if (DEBUG_OUTPUT > 0)
        cout << endl << GetNameByIndex(node.shape) << "  " << "rotation: " << matchingRotation << endl;

    auto shapeDesc = shapeDescriptors.find(node.shape);
    if (shapeDesc != shapeDescriptors.end()) {
        auto *shapeDescriptor = shapeDesc->second.get();
        float t = 0;
        vector<ShapeIndex> pattern;
        if (COMPOSED_SHAPES_ENABLED) {
            while (t < 1) {
                ImageLines copy = imageLines;
                float2 point = shapeDescriptor->GetPoint(t);
                point = RotatePoint(point, matchingRotation, float2{0.5f, 0.5f});
                copy.Clip(point - float2{COMPOSITION_WINDOW_SIZE, COMPOSITION_WINDOW_SIZE},
                          point + float2{COMPOSITION_WINDOW_SIZE, COMPOSITION_WINDOW_SIZE});
                pattern.push_back(AnalyzeImageLines(copy));

                t += 1 / (float)COMPOSITION_SAMPLES_COUNT;

                if (!copy.Empty() && DEBUG_IMAGE_SAVE) {
                    copy.Normalize();
                    DrawLines(copy, IMAGE_SIDE_SIZE, LINE_DRAWING_STEP_SIZE).SaveToFile(
                            "debug/pat" + to_string(randomizer.ratio()));
                }
            }
        }

        int count = 0;
        ShapeIndex patternShape = CountShapes(pattern, count);

        if (DEBUG_OUTPUT > 0)
            cout << GetNameByIndex(patternShape) << " " << count << endl;

        if (count > COMPOSITION_SAMPLES_LIMIT) {
            node.shapePattern = patternShape;
        }

        if (EMBEDDED_SHAPES_ENABLED) {
            for (float3 poi : shapeDescriptor->GetPointsOfInterest()) {
                ImageLines copy = imageLines;
                copy.Transform(CreateRotationMatrix(matchingRotation, float2(0.5f, 0.5f)));
                copy.Normalize();
                if (DEBUG_IMAGE_SAVE) {
                    DrawLines(copy, IMAGE_SIDE_SIZE, LINE_DRAWING_STEP_SIZE).SaveToFile(
                            "debug/r" + to_string(randomizer.ratio()));
                }
                copy.Clip(float2(poi.x, poi.y), float2(poi.x, poi.y) + float2(poi.z, poi.z));
                node.childNodes.push_back(Analyze(copy));
            }
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
        cout << "ERROR: attempt to register uninitialized index or UNKNOWN_SHAPE index" << endl;
    }
    shapeDescriptors.emplace(index, move(shapeDescriptor));
}

