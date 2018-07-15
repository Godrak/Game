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

        virtual void Train(int epochCount);

        void LoadData(const string &filename);

        float Test(fann_train_data *data);

        void Save(const string &filename);

        void SetError(float error){
           desiredError = error;
       }

        virtual ~TrainingCase();
    };
}