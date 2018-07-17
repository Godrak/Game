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
    ImageAnalyzer::ROTATIONS_ENABLED = true;

    Training::COMPOSED_SHAPE_PROBABILITY = 0.3f;
    Training::FIRST_LEVEL_EMBEDDED_SHAPE_PROBABILITY = 0.5f;
    Training::SECOND_LEVEL_EMBEDDED_SHAPE_PROBABILITY = 0.2f;

//    GenerateData("test.data", 40000, 20000, false, false);
//    GenerateData("training.data", 150000, 50000, false, false);
//    GenerateData("overallWithRotation.data", 1000, 0, true, false);

//    Training::Train("newGC01",vector<unsigned int>{1024, 300,30,4},true,0.1f);
    Training::Train("network",vector<unsigned int>{1024, 300,30,4},false,0.01f);
    return 0;
}