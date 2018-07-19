#include <glob.h>
#include <sstream>
#include "Training.h"
#include "../ImageAnalyzer/ExampleShapeDescriptors.h""


///helper functions, skip to main
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

void toString(const ShapeNode &shapeNode, string &result) {
    string shape, pattern;
    if (shapeNode.shape == ShapeIndex(0))
        shape = "square";
    else if (shapeNode.shape == ShapeIndex(1))
        shape = "circle";
    else if (shapeNode.shape == ShapeIndex(2))
        shape = "triangle";
    else if (shapeNode.shape == ShapeIndex(3))
        shape = "water";
    else
        return;

    if (shapeNode.shapePattern == ShapeIndex(0))
        pattern = "square";
    else if (shapeNode.shapePattern == ShapeIndex(1))
        pattern = "circle";
    else if (shapeNode.shapePattern == ShapeIndex(2))
        pattern = "triangle";
    else if (shapeNode.shapePattern == ShapeIndex(3))
        pattern = "water";
    else
        pattern = "unknown";

    result += "_" + shape + "_" + pattern;
    if (!shapeNode.childNodes.empty()) {
        toString(shapeNode.childNodes[0], result);
    }
}


int main() {

    ///using the namespace Training from Training.h, where all the training stuff is located.
    using namespace Training;


    ///Registering the shape descriptors that will be used for training. See ImageAnalyzer/ExampleShapeDescriptors.h
    /// for implementation and ImageAnalyzer.h for definition and description. saving the indexes
    auto square = RegisterShapeDescriptor(move(unique_ptr<SquareDescriptor>(new SquareDescriptor())));
    auto circle = RegisterShapeDescriptor(move(unique_ptr<CircleDescriptor>(new CircleDescriptor())));
    auto triangle = RegisterShapeDescriptor(move(unique_ptr<TriangleDescriptor>(new TriangleDescriptor())));
    auto waterDrop = RegisterShapeDescriptor(move(unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor())));

    ///setting up the algorithm parameters. They influence what the network is actually trained to recognize.
    /// Only these three actually influence the training algorithm.
    ImageAnalyzer::EMBEDDED_SHAPES_ENABLED = true;
    ImageAnalyzer::COMPOSED_SHAPES_ENABLED = true;
    ImageAnalyzer::ROTATIONS_ENABLED = true;

    ///Generating the small portion of data and setting the generateImage and saveImageLines to true.
    /// generateImage parameter causes the training algorithm to dump the training images in bmp format to folder bin/generated
    /// which can be then viewed.
    /// saveImageLines parameter causes the algorithm to dump ImageLines instances into the /bin/generated folder,
    ///  these can be later loaded and used for debugging purposes
    /// the data file is saved in the bin folder.
    GenerateData("NotUsed.data", 100, 50, true, true);

    /// training the network with current shape descriptors and settings. The network is then saved to bin folder in file "name".net
    /// the vector of unisgned integers represents the network layers from input to output. Note that the first layer has to be equal to IMAGE_ANALYZER::IMAGE_SIDE_SIZE^2,
    /// and the last to the number of shape descriptors registered.
    /// generateData parameter causes the algorithm to generate two data files: test.data and training.data in the bin folder.
    /// if the generateData parameter is false, the algorithm will look for the files in the bin folder, and if they are not found, ends with error.
//    Training::Train("network",vector<unsigned int>{1024, 300,30,4},true,0.01f);

    ///example of how the generated data can be reused
    //Training::Train("network2",vector<unsigned int>{1024, 300,30,4},false,0.01f);

    //that is end of training
    // the usage:

    using namespace ImageAnalyzer;

    ///usually the training is done only once while the using is repeated. thats why we set the algorithm params again, because
    /// normally the training and usage parts will be probably in different executables. See GameSource/SpellSystem.h for example usage
    EMBEDDED_SHAPES_ENABLED = true;
    COMPOSED_SHAPES_ENABLED = true;
    ROTATIONS_ENABLED = true;

    ///we can set other parameters arbitrarily, with sensible values! se the appendix for more info. These are default values
    DEBUG_IMAGE_SAVE_SIDE_SIZE = 128;
    COMPOSITION_SAMPLES_COUNT = 40;
    COMPOSITION_SAMPLES_LIMIT = 1;
    COMPOSITION_WINDOW_SIZE = 0.08f;
    ROTATION_SAMPLES_COUNT = 13;
    DEBUG_OUTPUT = 1;
    IMAGE_SIDE_SIZE = 32;
    RECURSION_LIMIT = 3;
    SHAPE_VALUE_LIMIT = 0.6;

    ///If true, this parameter causes every produced image to be saved to harddisk, to folder bin/debug, before the image is presented to the neural network
    DEBUG_IMAGE_SAVE = false;

    ///register the same shape descriptors! with the corresponding ShapeIndex. Because the ShapeIndexes cannot be saved to harddisk or otherwise preserved
    /// for future usage, there is an alternative way how to re-create them:
//    circle = ShapeIndex(1);
//    square = ShapeIndex(0);
//    waterDrop = ShapeIndex(3);
//    triangle = ShapeIndex(2);
    /// the shape index can be reconstructed by using an order number startng from zero in which it was registered in the training process,
    /// for example, the square was registered first in the training, so its ShapeIndex can be constructed from 0. the waterDrop shape descriptor
    /// was registered last (fourth), so it can be reconstructed from 3 etc.

    ImageAnalyzer::RegisterShapeDescriptor(square, std::unique_ptr<SquareDescriptor>(new SquareDescriptor()));
    ImageAnalyzer::RegisterShapeDescriptor(circle, std::unique_ptr<CircleDescriptor>(new CircleDescriptor()));
    ImageAnalyzer::RegisterShapeDescriptor(triangle, std::unique_ptr<TriangleDescriptor>(new TriangleDescriptor()));
    ImageAnalyzer::RegisterShapeDescriptor(waterDrop, std::unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor()));

    ///loading the trained neural network. the argument is the path to the network.
    ImageAnalyzer::LoadNetwork("../network.net");

    //end of setup, the algorithm is ready

    ///To demonstrate the functionality, we reuse the ImageLines instances created in the training process on line 20.
    /// Note that if you change the shape descriptors or their order, the following part will probably not work correctly.
    /// however changes in the netowrks structure and in the algorithm settings are ok.

    /// we switch of the debug output, otherwise the terminal becomes cluttered soon
    ImageAnalyzer::DEBUG_OUTPUT = 0;

    /// globbing all .lines files

    vector<string> squares = glob("generated/square*.lines");
    vector<string> circles = glob("generated/circle*.lines");
    vector<string> triangles = glob("generated/triangle*.lines");
    vector<string> drops = glob("generated/water*.lines");

    // showing an example for squares only, can be copied for each shape
    for (auto squarePath : squares) {
        ImageLines instance;
        ///loading the image lines
        instance.LoadFromFile(squarePath);

        ///analyzing the image lines
        auto result = ImageAnalyzer::Analyze(instance);

        ///serializing the output to vector of string
        string resultSerialized = "algo";
        toString(result, resultSerialized);
        auto analysis = split(resultSerialized);

        ///serializing the expected output to vector of strings - the shape strucuture is encoded in the file name
        squarePath.erase(squarePath.find('.'), 6);
        auto input = split(squarePath);


        cout << "Input:\t";
        for (int i = 1; i < input.size(); ++i) {
            cout << input[i] << "\t";
        }
        cout << endl;


        cout << "Analysis:\t";
        for (int i = 1; i < analysis.size(); ++i) {
            cout << analysis[i] << "\t";
        }
        cout << endl;

        std::cout << "------------------------" << endl;
    }


    return 0;
}