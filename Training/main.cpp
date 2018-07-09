#include "Training.h"
#include "../ImageAnalyzer/ExampleShapeDescriptors.h"


void StructureTraining() {
    using namespace Training;

    vector<unsigned int> layerOne = {100, 200, 300};
    vector<unsigned int> layerTwo = {10, 20, 30};
    vector<float> targetMse = {0.1, 0.07, 0.04, 0.01};

    for (auto mse : targetMse) {
        for (auto layerOneCount : layerOne) {
            for (auto layerTwoCount : layerTwo) {
                TrainingCase trainingCase(vector<unsigned int>{1024, layerOneCount, layerTwoCount, 4});
                auto finalMse = Training::Train(trainingCase, false, mse);
                trainingCase.Save("network" + to_string(layerOneCount) + "_" + to_string(layerTwoCount)
                                 +"_mse:" + to_string(finalMse) + "embedded:"+to_string(ImageAnalyzer::EMBEDDED_SHAPES_ENABLED)
                                  + "composed:" + to_string(ImageAnalyzer::COMPOSED_SHAPES_ENABLED) + ".net");
            }
        }
    }
};

int main2() {
    using namespace Training;

    RegisterShapeDescriptor(move(unique_ptr<SquareDescriptor>(new SquareDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<CircleDescriptor>(new CircleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<TriangleDescriptor>(new TriangleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor())));

    ImageAnalyzer::EMBEDDED_SHAPES_ENABLED = false;
    ImageAnalyzer::COMPOSED_SHAPES_ENABLED = true;
    ImageAnalyzer::ROTATIONS_ENABLED = true;
//    GenerateData("test.data", 40000, 20000, false, false);
//    GenerateData("training.data", 150000, 50000, false, false);
    GenerateData("rotation.data", 1000, 0, true, true);
//    StructureTraining();
    return 0;
}