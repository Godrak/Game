#include "Training.h"
#include "../ImageAnalyzer/ExampleShapeDescriptors.h"


void StructureTraining() {
    using namespace Training;

    vector<unsigned int> layerOne = {100,200,300};
    vector<unsigned int> layerTwo = {10,20,30};

    for (auto layerOneCount : layerOne) {
        for (auto layerTwoCount : layerTwo) {
            TrainingCase trainingCase(vector<unsigned int>{1024, layerOneCount, layerTwoCount, 4});
            auto mse = Training::Train(trainingCase, false);
            trainingCase.Save(to_string(mse)+"network" + to_string(layerOneCount) + "_" + to_string(layerTwoCount)
                              + ".net");
        }
    }
};

int main() {
    using namespace Training;

    RegisterShapeDescriptor(move(unique_ptr<SquareDescriptor>(new SquareDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<CircleDescriptor>(new CircleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<TriangleDescriptor>(new TriangleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor())));

//    GenerateData("hello.data",400,200,true);
//    Train("network.net");
    StructureTraining();
    return 0;
}