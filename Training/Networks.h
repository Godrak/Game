#pragma once

#include <floatfann.h>
#include <fstream>
#include "../ImageAnalyzer/GrayScaleImage.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"

namespace Training {

    class TrainingCase {
    protected:
        fann *ann;
        fann_train_data *data = nullptr;
        float desiredError = 0.03f;
        TrainingCase(){};
    public:
        explicit TrainingCase(vector<unsigned int> networkStructure);

        TrainingCase(string fileName) {
            ann = fann_create_from_file(fileName.c_str());
        }

        virtual void Train(int epochCount);

        virtual void Train(fann_train_data *data, int epochCount);

        void LoadData(const string &filename);

        int Test(string dataName);

        float Test(fann_train_data *data);

        void Save(const string &filename);

        void SetLearningParams(float learningRate, float learningMomentum);

       void SetError(float error){
           desiredError = error;
       }

        virtual ~TrainingCase();
    };

/*
    class CascadeTrainingCase:public TrainingCase {
    public:
        explicit CascadeTrainingCase(vector<unsigned int> networkStructure) {
            ann = fann_create_shortcut_array(networkStructure.size(), networkStructure.data());
            fann_set_train_stop_function(ann, FANN_STOPFUNC_MSE);
        }

        void Train(int neurons) override {
            fann_cascadetrain_on_data(ann, data, neurons, 1, desiredError);
        }

    };
    */
}