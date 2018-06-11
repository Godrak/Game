#pragma once

#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "../ImageAnalyzer/LineDrawer.h"
#include "Networks.h"


using namespace std;
using namespace ImageAnalyzer;
using namespace LineDrawer;

namespace Training {

    ShapeIndex RegisterShapeDescriptor(unique_ptr<ShapeDescriptor> shapeDescriptor);

    void GenerateData(const std::string &filename, int validDataCount,
                      int invalidDataCount, bool generateImages);

    float Train(TrainingCase &trainingCase, bool generateData = true, float targetMSE =0.05, int dataSize = 300000);

//    void Train(string networkFile);

//    void ManualTraining(string networkFile);
}
