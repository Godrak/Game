#include "Training.h"
#include "../ImageAnalyzer/ExampleShapeDescriptors.h"

int main() {
    using namespace Training;

    RegisterShapeDescriptor(move(unique_ptr<SquareDescriptor>(new SquareDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<CircleDescriptor>(new CircleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<TriangleDescriptor>(new TriangleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor())));

    ImageAnalyzer::EMBEDDED_SHAPES_ENABLED = true;
    ImageAnalyzer::COMPOSED_SHAPES_ENABLED = true;
    ImageAnalyzer::ROTATIONS_ENABLED = false;
//    GenerateData("test.data", 40000, 20000, false, false);
//    GenerateData("training.data", 150000, 50000, false, false);
//    GenerateData("overallWithRotation.data", 1000, 0, true, false);

    Training::Train("newGC",vector<unsigned int>{1024, 100,10,4},false);
    return 0;
}