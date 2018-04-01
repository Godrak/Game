#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "../ImageAnalyzer/LineDrawer.h"


using namespace ImageAnalyzer;
using namespace std;
using namespace LineDrawer;
using namespace ShapeDescriptors;


void DrawShape(const ShapeDescriptor* shapeDescriptor) {
    float t = 0;
    vector<Line> lines;
    float2 lastPoint = shapeDescriptor->GetPoint(t);
    while (t < 0.99) {
        t += 0.001;
        float2 nextPoint = shapeDescriptor->GetPoint(t);
        lines.emplace_back(lastPoint, nextPoint);
        lastPoint = nextPoint;
    }

    auto image = ImageLines(lines);

    DrawLines(image).SaveToFile("debug/" + ShapeToString(shapeDescriptor->GetName()));

};


void ComposeShapes(const ShapeDescriptor &base, const ShapeDescriptor &modifier, int multiplication) {
    float t = 0;
    vector<Line> lines;
    float2 lastBasePoint = base.GetPoint(t);
    float2 lastPoint = lastBasePoint;

    float2 nextBasePoint;
    float2 offset;
    float2 shapeNormal;
    float2 nextPoint;
    while (t < 0.99) {
        t += 0.001;
        nextBasePoint = base.GetPoint(t);
        offset = modifier.GetPoint(t * multiplication);
        shapeNormal = {lastBasePoint.y - nextBasePoint.y, nextBasePoint.x - lastBasePoint.x};
        if (length(shapeNormal) > 0) {
            shapeNormal = normalize(shapeNormal);
            float length = (offset.y - 0.5f) * 1.5f / multiplication;
            nextPoint = MovePoint(nextBasePoint, length * shapeNormal);
        } else {
            nextPoint = nextBasePoint;
        }
        lines.emplace_back(lastPoint, nextPoint);
        lastBasePoint = nextBasePoint;
        lastPoint = nextPoint;
    }

    auto image = ImageLines(lines);
    image.Normalize();
    DrawLines(image).SaveToFile(
            "debug/base:" + ShapeToString(base.GetName()) + ":mod:" + ShapeToString(modifier.GetName()) +
            to_string(multiplication));
};


int main() {
    int mul = 15;

    for( const ShapeDescriptor* d : SHAPE_DESCRIPTORS) {
        DrawShape(d);
    }    
//    
//    ComposeShapes(rectangle, rectangle, mul);
//    ComposeShapes(rectangle, circle, mul);
//    ComposeShapes(rectangle, triangle, mul);
//    ComposeShapes(rectangle, line, mul);
//    ComposeShapes(rectangle, serrate, mul);
//    ComposeShapes(rectangle, wavy, mul);

    return 0;
};