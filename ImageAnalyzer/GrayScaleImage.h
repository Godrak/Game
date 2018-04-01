#pragma once

#include <vector>
#include <iostream>

#include "../EasyBMP_1.06/EasyBMP_DataStructures.h"
#include "../EasyBMP_1.06/EasyBMP_BMP.h"

using namespace std;

class GrayScaleImage {
private:
    vector<float> pixelValueVector;
    int size;
public:

    GrayScaleImage(int size) : size(size) {
        for (int i = 0; i < size * size; ++i) {
            pixelValueVector.push_back(0);
        }
    }

    int GetSize() const {
        return size;
    }

    float &PixelValueAt(int x, int y) {
        BoundCheck(x, y);
        return pixelValueVector[x + y * size];
    }

    const float &PixelValueAt(int x, int y) const {
        BoundCheck(x, y);
        return pixelValueVector[x + y * size];
    }

    vector<float> Serialize() const {
        return pixelValueVector;
    }

    void Clear() {
        for (int i = 0; i < pixelValueVector.size(); ++i) {
            pixelValueVector[i] = 0;
        }
    }

    void SaveToFile(const string &filename) {
        BMP bitmap;
        bitmap.SetSize(size, size);
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                bitmap.SetPixel(x, y, FloatToRGB(PixelValueAt(x, y)));
            }
        }
        bitmap.WriteToFile(filename.c_str());
    }

    GrayScaleImage(const string &filename) {
        BMP file;
        file.ReadFromFile(filename.c_str());
        size = file.TellWidth();

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                pixelValueVector.push_back(GetValueFromRGB(file.GetPixel(x, y)));
            }
        }
    }

    GrayScaleImage(const GrayScaleImage &image) {
        size = image.GetSize();
        pixelValueVector = image.Serialize();
    }

    GrayScaleImage(const GrayScaleImage &origin, int topLeftCornerX, int topLeftCornerY, int size) : size(
            size) {
        for (int y = topLeftCornerY; y < topLeftCornerY + size; y++) {
            for (int x = topLeftCornerX; x < topLeftCornerX + size; x++) {
                pixelValueVector.push_back(origin.PixelValueAt(x, y));
            }
        }
    }


private:
    RGBApixel FloatToRGB(float value) {
        ebmpBYTE byte = (ebmpBYTE) (255 * value);
        return RGBApixel{byte, byte, byte, 0};
    }

    float GetValueFromRGB(RGBApixel pixel) {
        return pixel.Red / 255.0f;
    }

    void BoundCheck(int x, int y) const {
        if (x <0 || x>= size || y < 0 || y >= size){
            cout << "invalid access on x: " << x << "  y: " << y << endl;
        }
    }
};
