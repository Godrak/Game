
#include <glob.h>
#include "Networks.h"
#include "../ImageAnalyzer/LineDrawer.h"

using namespace ImageAnalyzer;
using namespace LineDrawer;

Training::TrainingCase::TrainingCase(string networkName, vector<unsigned int> networkStructure)
        : networkName(networkName) {

    ann = fann_create_sparse_array(1, networkStructure.size(), networkStructure.data());

    fann_set_activation_function_hidden(ann, FANN_ELLIOT);
    fann_set_training_algorithm(ann, FANN_TRAIN_INCREMENTAL);
    fann_set_train_stop_function(ann, FANN_STOPFUNC_BIT);
}

void Training::TrainingCase::Train(int epochCount) {
    fann_train_on_data(ann, data, epochCount, 1, 0.f);
}

void Training::TrainingCase::LoadData(const string &filename) {
    if (data != nullptr) {
        fann_destroy_train(data);
    }

    data = fann_read_train_from_file(("batches/" + filename).c_str());
}

int Training::TrainingCase::Test(string dataName) {
    auto *testData = fann_read_train_from_file(dataName.c_str());
    int BF = Test(testData);
    fann_destroy_train(testData);
    return BF;
}

int Training::TrainingCase::Test(fann_train_data *data) {
    fann_reset_MSE(ann);
    fann_test_data(ann, data);
    float MSE = fann_get_MSE(ann);
    int BF = fann_get_bit_fail(ann);
    cout << ":  MSE: " << to_string(MSE) << "  bitFail: " << to_string(BF) << endl;
    return BF;
}


void Training::TrainingCase::Save(const string &filename) {
    fann_save(ann, filename.c_str());
}

void Training::TrainingCase::SetLearningParams(float learningRate, float learningMomentum) {
    fann_set_learning_rate(ann, learningRate);
    fann_set_learning_momentum(ann, learningMomentum);

}

Training::TrainingCase::~TrainingCase() {
    if (data != NULL) {
        fann_destroy_train(data);
    }

    if (ann != NULL) {
        fann_destroy(ann);
    }
}

void Training::TrainingCase::TrainOnAllBatches(int epochCount) {
    vector<string> batches;
    glob_t globResult;
    glob("batches/*", GLOB_TILDE, NULL, &globResult);
    for (int i = 0; i < globResult.gl_pathc; ++i) {
        batches.emplace_back(globResult.gl_pathv[i]);
    }
    for (const auto &batch : batches) {
        LoadData(batch.data() + 8);
        Train(epochCount);
    }
}

//void Training::RenameTestFiles(string networkFile) {
//    vector<string> testFiles;
//    glob_t globResult;
//    glob("test/*", GLOB_TILDE, NULL, &globResult);
//    for (int i = 0; i < globResult.gl_pathc; ++i) {
//        testFiles.emplace_back(globResult.gl_pathv[i]);
//    }
//    vector<GrayScaleImage> testData;
//    NeuralNetwork network(networkFile.c_str());
//
//    for (auto &testFile : testFiles) {
//        GrayScaleImage test(testFile);
//        vector<float> result = network.Calculate(test.Serialize());
//
//        int maxIndex = -1;
//        float maxValue = 0.65;
//        for (int i = 0; i < result.size(); ++i) {
//            if (result[i] > maxValue) {
//                maxValue = result[i];
//                maxIndex = i;
//            }
//        }
//        test.SaveToFile("debug/" + ShapeToString(Shape(maxIndex)) + to_string(RANDOMIZER.ratio()) + ".bmp");
//    }
//
//}

//void Training::GenerateTestData() {
//    vector<string> testFiles;
//    glob_t globResult;
//    glob("testFiles/*", GLOB_TILDE, NULL, &globResult);
//    for (int i = 0; i < globResult.gl_pathc; ++i) {
//        testFiles.emplace_back(globResult.gl_pathv[i]);
//    }
//    vector<GrayScaleImage> testData;
//
//    ofstream trainingData("test.data");
//    trainingData << testFiles.size() << " " << NETWORK_INPUT_SIZE << " "
//                 << NETWORK_OUTPUT_SIZE << std::endl;
//
//    for (auto &testFile : testFiles) {
//        GrayScaleImage test(testFile);
//
//        for (float grayScale : test.Serialize()) {
//            trainingData << grayScale << " ";
//        }
//
//        Shape shape = DetermineCorrectShape(testFile, 10);
//
//        trainingData << std::endl;
//        //output shape
//        for (int j = 0; j < NETWORK_OUTPUT_SIZE; ++j) {
//            trainingData << (j == (int) shape ? 1 : 0);
//            trainingData << " ";
//        }
//
//        trainingData << std::endl;
//    }
//    trainingData.close();
//}
//
//void Training::TestNeurals() {
//    vector<string> neurals;
//    glob_t globResult;
//    glob("neurals/*", GLOB_TILDE, NULL, &globResult);
//    for (int i = 0; i < globResult.gl_pathc; ++i) {
//        neurals.emplace_back(globResult.gl_pathv[i]);
//    }
//
//    auto *data = fann_read_train_from_file("test.data");
//
//    for (const auto &n : neurals) {
//        auto *network = fann_create_from_file(n.c_str());
//        fann_reset_MSE(network);
//        for (int i = 0; i != data->num_data; i++) {
//            fann_test(network, data->input[i], data->output[i]);
//        }
//        float MSE = fann_get_MSE(network);
//        float BF = fann_get_bit_fail(network);
//        cout << n << ":  MSE: " << to_string(MSE) << "  bitFail: " << to_string(BF) << endl;
//        fann_destroy(network);
//    }
//    fann_destroy_train(data);
//
//};
