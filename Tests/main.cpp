#include <glob.h>
#include <sstream>
#include <chrono>
#include <utility>
#include "../Training/Training.h"
#include "../ImageAnalyzer/ExampleShapeDescriptors.h"


const ShapeIndex square{0};
const ShapeIndex circle{1};
const ShapeIndex triangle{2};
const ShapeIndex drop{3};

vector<string> glob(string pattern) {
    vector<string> files;
    glob_t globResult;
    glob(pattern.c_str(), GLOB_TILDE, NULL, &globResult);
    for (int i = 0; i < globResult.gl_pathc; ++i) {
        files.emplace_back(globResult.gl_pathv[i]);
    }
    return files;
}

vector<string> split(string s) {
    std::stringstream test(s);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(test, segment, '_')) {
        seglist.push_back(segment);
    }

    return seglist;
}

void testNetworks() {
    using namespace Training;

    vector<string> networks = glob("../Tests/nets/*");

    for (const auto &network : networks) {
        cout << network << endl;
    }

    cout << networks.size();

    ImageAnalyzer::EMBEDDED_SHAPES_ENABLED = true;
    ImageAnalyzer::COMPOSED_SHAPES_ENABLED = true;

    RegisterShapeDescriptor(move(unique_ptr<SquareDescriptor>(new SquareDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<CircleDescriptor>(new CircleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<TriangleDescriptor>(new TriangleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor())));

    Training::GenerateData("testing.data", 1000, 0, true);

    vector<string> squares = glob("generated/square*");
    vector<string> circles = glob("generated/circle*");
    vector<string> triangles = glob("generated/triangle*");
    vector<string> drops = glob("generated/water*");

    ofstream results{"composedShapesTestsResults.txt"};

    NeuralNetwork net;
    for (const auto &network : networks) {
        net.Load(network.c_str());

        for (const auto &square : squares) {
            GrayScaleImage c{square};
            auto input = c.Serialize();
            auto res = net.Calculate(input);
            results << network << " " << 0 << " ";
            for (const auto &r : res) {
                results << r << " ";
            }
            results << endl;
        }

        for (const auto &circle : circles) {
            GrayScaleImage c{circle};
            auto input = c.Serialize();
            auto res = net.Calculate(input);
            results << network << " " << 1 << " ";
            for (const auto &r : res) {
                results << r << " ";
            }
            results << endl;
        }

        for (const auto &triangle : triangles) {
            GrayScaleImage c{triangle};
            auto input = c.Serialize();
            auto res = net.Calculate(input);
            results << network << " " << 2 << " ";
            for (const auto &r : res) {
                results << r << " ";
            }
            results << endl;
        }

        for (const auto &drop : drops) {
            GrayScaleImage c{drop};
            auto input = c.Serialize();
            auto res = net.Calculate(input);
            results << network << " " << 3 << " ";
            for (const auto &r : res) {
                results << r << " ";
            }
            results << endl;
        }
    }
}

void toString(const ShapeNode &shapeNode, string &result) {
    string shape, pattern;
    if (shapeNode.shape == square)
        shape = "square";
    else if (shapeNode.shape == circle)
        shape = "circle";
    else if (shapeNode.shape == triangle)
        shape = "triangle";
    else if (shapeNode.shape == drop)
        shape = "water";
    else
        return;

    if (shapeNode.shapePattern == square)
        pattern = "square";
    else if (shapeNode.shapePattern == circle)
        pattern = "circle";
    else if (shapeNode.shapePattern == triangle)
        pattern = "triangle";
    else if (shapeNode.shapePattern == drop)
        pattern = "water";
    else
        pattern = "zero";

    result += "_" + shape + "_" + pattern;
    if (!shapeNode.childNodes.empty()) {
        toString(shapeNode.childNodes[0], result);
    }


}

string access(vector<string> s, int i) {
    if (i >= s.size()) return "";
    return s[i];
}

bool compositionComparisonEnabled = false;
bool normalComparisonEnabled = true;
int comparisons;
int correctComparisons;

long fullDuration = 0;
long peakDuration = 0;

void compare(string image, string analysis) {
    auto test = split(image);
    auto result = split(analysis);

    for (int i = 1; i < max(test.size(), result.size()); i += 2) {
        if (normalComparisonEnabled) {
            if (access(test, i) == access(result, i))
                correctComparisons++;
            comparisons++;
        }

        if (compositionComparisonEnabled) {
            if (access(test, i + 1) == access(result, i + 1))
                correctComparisons++;
            comparisons++;
        }
    }
}

void composition() {
    using namespace ImageAnalyzer;
    LoadNetwork("../network.net");

    ImageAnalyzer::COMPOSED_SHAPES_ENABLED = true;
    ImageAnalyzer::EMBEDDED_SHAPES_ENABLED = false;
    ImageAnalyzer::ROTATIONS_ENABLED = false;
    ImageAnalyzer::DEBUG_OUTPUT = 0;
    ImageAnalyzer::DEBUG_IMAGE_SAVE = false;
    ImageAnalyzer::ROTATION_SAMPLES_COUNT = 1;

    ImageAnalyzer::RegisterShapeDescriptor(square, std::unique_ptr<SquareDescriptor>(new SquareDescriptor()));
    ImageAnalyzer::RegisterShapeDescriptor(circle, std::unique_ptr<CircleDescriptor>(new CircleDescriptor()));
    ImageAnalyzer::RegisterShapeDescriptor(triangle, std::unique_ptr<TriangleDescriptor>(new TriangleDescriptor()));
    ImageAnalyzer::RegisterShapeDescriptor(drop, std::unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor()));

    auto imageLinesTests = glob("generated/*.lines");
    ofstream performance("performance.txt");
    performance << imageLinesTests.size() << endl;

    for (int i = 20; i < 61; i += 5) {
        for (int j = 1; j < 4; j++) {
            for (int k = 0; k < 4; ++k) {
                comparisons = 0;
                correctComparisons = 0;
                fullDuration = 0;
                peakDuration = 0;
                ImageAnalyzer::COMPOSITION_SAMPLES_COUNT = i;
                ImageAnalyzer::COMPOSITION_SAMPLES_LIMIT = j;
                ImageAnalyzer::COMPOSITION_WINDOW_SIZE = 0.1f + k * 0.04f;


                for (const auto &test : imageLinesTests) {
                    ImageLines t{};
                    t.LoadFromFile(test);

                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
                    auto res = Analyze(t);
                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                    fullDuration += duration;
                    peakDuration = max(peakDuration, duration);

//                  auto i = DrawLines(t, 128, LINE_DRAWING_STEP_SIZE);
//                  i.SaveToFile("debug/" + test);

                    string analysis = "algo";
                    toString(res, analysis);
                    auto correct = test;
                    correct.erase(correct.find('.'), 6);
                    compare(correct, analysis);

                }

                cout << endl << i << " " << j << " " << 0.04f + k * 0.04f;
                cout << endl << correctComparisons << " " << comparisons << " ,time: "
                     << fullDuration / (float) imageLinesTests.size();
                performance << comparisons << " " << correctComparisons << " " << fullDuration << " " << peakDuration;
                performance << " " << i << " " << j << " " << 0.04f + k * 0.04f << endl;
            }
        }
    }
}

int rotationTests() {
    using namespace ImageAnalyzer;
    LoadNetwork("../network.net");

    ImageAnalyzer::COMPOSED_SHAPES_ENABLED = false;
    ImageAnalyzer::EMBEDDED_SHAPES_ENABLED = false;
    ImageAnalyzer::ROTATIONS_ENABLED = true;
    ImageAnalyzer::DEBUG_OUTPUT = 0;
    ImageAnalyzer::DEBUG_IMAGE_SAVE = false;
    ImageAnalyzer::ROTATION_SAMPLES_COUNT = 1;
    ImageAnalyzer::COMPOSITION_SAMPLES_COUNT = 1;
    ImageAnalyzer::COMPOSITION_SAMPLES_LIMIT = 1;
    ImageAnalyzer::COMPOSITION_WINDOW_SIZE = 0.04;

    ImageAnalyzer::RegisterShapeDescriptor(square, std::unique_ptr<SquareDescriptor>(new SquareDescriptor()));
    ImageAnalyzer::RegisterShapeDescriptor(circle, std::unique_ptr<CircleDescriptor>(new CircleDescriptor()));
    ImageAnalyzer::RegisterShapeDescriptor(triangle, std::unique_ptr<TriangleDescriptor>(new TriangleDescriptor()));
    ImageAnalyzer::RegisterShapeDescriptor(drop, std::unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor()));

    auto imageLinesTests = glob("generated/*.lines");
    ofstream rotations("rotationComp.txt");
    rotations << imageLinesTests.size() << endl;

    for (int i = 3; i < 31; i += 3) {
        ImageAnalyzer::ROTATION_SAMPLES_COUNT = i;
        comparisons = 0;
        correctComparisons = 0;
        fullDuration = 0;
        peakDuration = 0;

        for (const auto &test : imageLinesTests) {
            ImageLines t{};
            t.LoadFromFile(test);
            t.Transform(CreateRotationMatrix(hash<string>{}(test)*i/float(SIZE_MAX),float2{0.5,0.5}));

            std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
            auto res = Analyze(t);
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
            fullDuration += duration;
            peakDuration = max(peakDuration, duration);

//                  auto i = DrawLines(t, 128, LINE_DRAWING_STEP_SIZE);
//                  i.SaveToFile("debug/" + test);

            string analysis = "algo";
            toString(res, analysis);
            auto correct = test;
            correct.erase(correct.find('.'), 6);
            compare(correct, analysis);

        }

        cout << endl << i;
        cout << endl << correctComparisons << " " << comparisons << " ,time: "
             << fullDuration / (float) imageLinesTests.size();
        rotations << comparisons << " " << correctComparisons << " " << fullDuration << " " << peakDuration;
        rotations << " " << i << endl;
    }

    return 0;
}

int main2(){

}