#pragma once

#include <floatfann.h>
#include <fstream>
#include "../ImageAnalyzer/GrayScaleImage.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"

namespace Training {

    class TrainingCase {
        string networkName;
        fann *ann;
        fann_train_data *data = nullptr;
    public:
        explicit TrainingCase(string networkName, vector<unsigned int> networkStructure);

        TrainingCase(string fileName) {
            ann = fann_create_from_file(fileName.c_str());
        }

        string GetName() {
            return networkName;
        }

        void Train(int epochCount);

        void LoadData(const string &filename);

        int Test(string dataName);

        int Test(fann_train_data *data);

        void Save(const string &filename);

        void SetLearningParams(float learningRate, float learningMomentum);

        void TrainOnAllBatches(int epochCount);

        virtual ~TrainingCase();
    };


    class CascadeTrainingCase {
        string networkName;
        vector<unsigned int> networkStructure;
        fann *ann;
        fann_train_data *data = nullptr;
    public:
        explicit CascadeTrainingCase(string networkName, vector<unsigned int> networkStructure) {
            ann = fann_create_shortcut_array(networkStructure.size(), networkStructure.data());
            fann_set_train_stop_function(ann, FANN_STOPFUNC_BIT);
        }

        string GetName() {
            return networkName;
        }

        void Train(int neurons) {
            fann_cascadetrain_on_data(ann, data, neurons, 1, 0.f);
        }

        void LoadData(const string &filename) {
            if (data != nullptr) {
                fann_destroy_train(data);
            }
            data = fann_read_train_from_file(filename.c_str());
        }


        void Save(const string &filename) {
            fann_save(ann, filename.c_str());
        }
    };
}