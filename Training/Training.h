#pragma once

#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "../ImageAnalyzer/LineDrawer.h"
#include "Networks.h"


namespace Training {

    extern float FIRST_LEVEL_EMBEDDED_SHAPE_PROBABILITY;
    extern float SECOND_LEVEL_EMBEDDED_SHAPE_PROBABILITY;
    extern float COMPOSED_SHAPE_PROBABILITY;

    ShapeIndex RegisterShapeDescriptor(unique_ptr<ShapeDescriptor> shapeDescriptor);

    void GenerateData(const std::string &filename, int validDataCount,
                      int invalidDataCount, bool generateImages = false, bool saveImageLines = false);

    float Train(const std::string& name, vector<unsigned int> networkStructure, bool generateData = true, float targetMSE =0.01, int dataSize = 300000);
}
