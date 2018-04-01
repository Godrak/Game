#include "LineDrawer.h"

void LineDrawer::DrawLine(const Line &line, GrayScaleImage &image) {
    float2 startPoint = (float) (IMAGE_SIDE_SIZE - 1) * line.GetStart();
    float2 endPoint = (float) (IMAGE_SIDE_SIZE - 1) * line.GetEnd();

    float t = 0;
    float2 vector = endPoint - startPoint;
    float len = length(vector);
    vector = normalize(vector);
    while (t < len) {
        float2 point = startPoint + t * vector;
        int2 pixel = {(int) point.x, (int) point.y};

        image.PixelValueAt((int) pixel.x, (int) pixel.y) = 1;
        t += LINE_DRAWING_STEP_SIZE;
    }

}

GrayScaleImage LineDrawer::DrawLines(const ImageLines &imageLines) {
    GrayScaleImage image{IMAGE_SIDE_SIZE};
    for (const auto &normalizedLine : imageLines.GetImageLines()) {
        DrawLine(normalizedLine, image);
    }
    return image;
}

void LineDrawer::RandomizeData(GrayScaleImage &image) {
    for (int x = 0; x < image.GetSize(); ++x) {
        for (int y = 0; y < image.GetSize(); ++y) {
            if (image.PixelValueAt(x, y) < 0.5f)
                image.PixelValueAt(x, y) = image.PixelValueAt(x, y) + RANDOMIZER.ratio() * 0.1f;
            else
                image.PixelValueAt(x, y) = image.PixelValueAt(x, y) - RANDOMIZER.ratio() * 0.1f;
        }
    }
}
