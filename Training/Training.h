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

    };
}