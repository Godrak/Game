#pragma once

#include <floatfann.h>
#include <fstream>
#include "../ImageAnalyzer/GrayScaleImage.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "Constants.h"

namespace Training {

    void generateData(const string &filename, int validDataCount, int invalidDataCount, bool generateImages = false);

    class TrainingCase {
        string networkName;
        vector<unsigned int> networkStructure;
        fann *ann;
        fann_train_data *data = nullptr;
    public:
        explicit TrainingCase(string networkName, vector<unsigned int> networkStructure);

        string GetName(){
            return networkName;
        }

        void Train(int epochCount);

        void LoadData(const string &filename);

        void Test(ostream &output);

        void Save(const string &filename);

        void BatchTrain(uint validDataCount, uint invalidDataCount, uint epochsPerBatch, uint batchCount);

        void SetLearningParams(float learningRate, float learningMomentum);
    };


    class CascadeTrainingCase {
        string networkName;
        vector<unsigned int> networkStructure;
        fann *ann;
        fann_train_data *data = nullptr;
    public:
        explicit CascadeTrainingCase(string networkName, vector<unsigned int> networkStructure){
            ann = fann_create_shortcut_array(networkStructure.size(), networkStructure.data());
            fann_set_train_stop_function(ann, FANN_STOPFUNC_BIT);
        }

        string GetName(){
            return networkName;
        }

        void Train(int neurons){
            fann_cascadetrain_on_data(ann, data, neurons, 1, 0.f);
        }

        void LoadData(const string &filename){
            if (data != nullptr) {
                fann_destroy_train(data);
            }
            data = fann_read_train_from_file(filename.c_str());
        }


        void Save(const string &filename){
            fann_save(ann, filename.c_str());
        }

        void BatchTrain(uint validDataCount, uint invalidDataCount, uint neurons, uint batchCount){
            for (int i = 0; i < batchCount; ++i) {
                cout << "batch: " << i + 1 << " from " << batchCount << endl;
                generateData("batch.data", validDataCount, invalidDataCount);
                LoadData("batch.data");
                cout << "data generated" << endl;
                Train(neurons);
            }
        }

    };
}