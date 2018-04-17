#pragma once

#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "../ImageAnalyzer/LineDrawer.h"
#include "Networks.h"


using namespace std;
using namespace ImageAnalyzer;
using namespace LineDrawer;

namespace Training {

    ShapeIndex RegisterShapeDescriptor(unique_ptr<ShapeDescriptor> shapeDescriptor);

    void Train(string networkFile);
    
    void DrawDebugShapes();
    
    void ManualTraining(string networkFile);

    void GenerateData(const std::string &filename, int validDataCount,
                      int invalidDataCount, bool generateImages);
}
