#include "Training.h"
#include "../ImageAnalyzer/ExampleShapeDescriptors.h"

//
//void StructureTraining() {
//    using namespace Training;
//
//    int validData = 30000;
//    int invalidData = 10000;
//    int epochsCount = 10;
//    int batchCount = 15;
//    vector<string> trainingData;
//    string networkName;
//    fann_train_data *testData = fann_read_train_from_file("test.data");
//
//    for (int i = 0; i < batchCount; ++i) {
//        string batchName = "batch" + to_string(i) + ".data";
//        generateData(batchName, validData, invalidData);
//        trainingData.push_back(batchName);
//    }
//
//    vector<unsigned int> layerOne = {256, 192, 128};
//    vector<unsigned int> layerTwo = {32, 16, 8};
//
//    for (auto layerOneCount : layerOne) {
//        for (auto layerTwoCount : layerTwo) {
//            networkName = to_string(validData + invalidData) + "," + to_string(layerOneCount) + "," +
//                          to_string(layerTwoCount) + ".net";
//            cout << "TRAINING " << networkName << endl;
//            TrainingCase trainingCase(networkName,
//                                      vector<unsigned int>{NETWORK_INPUT_SIZE, layerOneCount, layerTwoCount,
//                                                           NETWORK_OUTPUT_SIZE});
//
//            trainingCase.SetLearningParams(0.01, 0);
//
//            for (const auto &fileName : trainingData) {
//                trainingCase.LoadData(fileName);
//                trainingCase.Train(epochsCount);
//                int bitFail = trainingCase.Test(testData);
//                trainingCase.Save(networkName + to_string(bitFail));
//            }
//        }
//    }
//};

int main() {
    using namespace Training;

    RegisterShapeDescriptor(move(unique_ptr<SquareDescriptor>(new SquareDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<CircleDescriptor>(new CircleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<TriangleDescriptor>(new TriangleDescriptor())));
    RegisterShapeDescriptor(move(unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor())));

    GenerateData("test.data",200,50,true);
//    Train("network.net");
    return 0;
}
