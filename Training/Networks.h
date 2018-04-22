#pragma once

#include <floatfann.h>
#include <fstream>
#include "../ImageAnalyzer/GrayScaleImage.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"

namespace Training {

    class TrainingCase {
    protected:
        string networkName;
        fann *ann;
        fann_train_data *data = nullptr;
        TrainingCase(){};
    public:
        explicit TrainingCase(string networkName, vector<unsigned int> networkStructure);

        TrainingCase(string fileName) {
            ann = fann_create_from_file(fileName.c_str());
        }

        string GetName() {
            return networkName;
        }

        virtual void Train(int epochCount);

        void LoadData(const string &filename);

        int Test(string dataName);

        float Test(fann_train_data *data);

        void Save(const string &filename);

        void SetLearningParams(float learningRate, float learningMomentum);

        void TrainOnAllBatches(int epochCount);

        virtual ~TrainingCase();
    };


    class CascadeTrainingCase:public TrainingCase {
    public:
        explicit CascadeTrainingCase(string networkName, vector<unsigned int> networkStructure) {
            ann = fann_create_shortcut_array(networkStructure.size(), networkStructure.data());
            fann_set_train_stop_function(ann, FANN_STOPFUNC_MSE);
        }

        void Train(int neurons) override {
            fann_cascadetrain_on_data(ann, data, neurons, 1, 0.05f);
        }

    };
}