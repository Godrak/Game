#include "LineDrawer.h"

void LineDrawer::DrawLine(const Line &line, GrayScaleImage &image, int imageSize, float drawingStepSize) {
    float2 startPoint = (float) (imageSize - 1) * line.GetStart();
    float2 endPoint = (float) (imageSize - 1) * line.GetEnd();

    float t = 0;
    float2 vector = endPoint - startPoint;
    float len = length(vector);
    vector = normalize(vector);
    while (t < len) {
        float2 point = startPoint + t * vector;
        int2 pixel = {(int) point.x, (int) point.y};

        image.PixelValueAt(pixel.x, pixel.y) = 1;
        t += drawingStepSize;
    }

}

GrayScaleImage LineDrawer::DrawLines(const ImageLines &imageLines, int imageSize, float drawingStepSize) {
    GrayScaleImage image{imageSize};
    for (const auto &normalizedLine : imageLines.GetImageLines()) {
        DrawLine(normalizedLine, image, imageSize, drawingStepSize);
    }
    return image;
}

void LineDrawer::RandomizeData(GrayScaleImage &image, Randomizer &randomizer) {
    for (int x = 0; x < image.GetSize(); ++x) {
        for (int y = 0; y < image.GetSize(); ++y) {
            if (image.PixelValueAt(x, y) < 0.5f)
                image.PixelValueAt(x, y) = image.PixelValueAt(x, y) + randomizer.ratio() * 0.05f;
            else
                image.PixelValueAt(x, y) = image.PixelValueAt(x, y) - randomizer.ratio() * 0.05f;
        }
    }
}
