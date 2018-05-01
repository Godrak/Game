
#include <glob.h>
#include "Networks.h"
#include "../ImageAnalyzer/LineDrawer.h"

using namespace ImageAnalyzer;
using namespace LineDrawer;

Training::TrainingCase::TrainingCase(vector<unsigned int> networkStructure) {

    ann = fann_create_sparse_array(1, networkStructure.size(), networkStructure.data());

    fann_set_activation_function_hidden(ann, FANN_ELLIOT);
    fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);
    fann_set_train_stop_function(ann, FANN_STOPFUNC_MSE);
}

void Training::TrainingCase::Train(int epochCount) {
    fann_train_on_data(ann, data, epochCount, 1, desiredError);
}

void Training::TrainingCase::Train(fann_train_data *data, int epochCount) {
    fann_train_on_data(ann, data, epochCount, 1, desiredError);
}

void Training::TrainingCase::LoadData(const string &filename) {
    if (data != nullptr) {
        fann_destroy_train(data);
    }

    data = fann_read_train_from_file((filename).c_str());
}

int Training::TrainingCase::Test(string dataName) {
    auto *testData = fann_read_train_from_file(dataName.c_str());
    int BF = Test(testData);
    fann_destroy_train(testData);
    return BF;
}


float Training::TrainingCase::Test(fann_train_data *data) {
    fann_reset_MSE(ann);
    fann_test_data(ann, data);
    float MSE = fann_get_MSE(ann);
    int BF = fann_get_bit_fail(ann);
    cout << ":  MSE: " << to_string(MSE) << "  bitFail: " << to_string(BF) << endl;
    return MSE;
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
