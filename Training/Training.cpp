#include "Training.h"

namespace { // training private
    bool inProgress = false;
    Randomizer randomizer{};
    vector<unique_ptr<ShapeDescriptor>> shapeDescriptors{};
}

namespace Training {
    float FIRST_LEVEL_EMBEDDED_SHAPE_PROBABILITY = 0.5f;
    float SECOND_LEVEL_EMBEDDED_SHAPE_PROBABILITY = 0.3f;
    float COMPOSED_SHAPE_PROBABILITY = 0.3f;
}

ShapeIndex Training::RegisterShapeDescriptor(unique_ptr<ShapeDescriptor> shapeDescriptor) {
    if (inProgress) {
        cout << "ERROR: cannot register new shape descriptor, training in progress" << endl;
        return UNKNOWN_SHAPE;
    }

    shapeDescriptors.push_back(move(shapeDescriptor));
    return ShapeIndex((int) shapeDescriptors.size() - 1);
}

ShapeDescriptor *RandomShapeDescriptor() {
    return shapeDescriptors[randomizer.next(shapeDescriptors.size())].get();
}

ImageLines
OffsetPoints(ShapeDescriptor *base, float offsetSize, float start, float momentum, float skipRatio, bool invalid) {
    float t = 0;
    float last_t = 0;
    vector<Line> lines;
    float2 lastBasePoint = base->GetPoint(t);
    float2 lastPoint = lastBasePoint;

    float2 nextBasePoint;
    float offset = 0;
    float target = 0;
    float step = 0;
    float2 shapeNormal;
    float2 nextPoint;
    while (t < 0.99) {
        t += 0.005;

        if (target == 0 && offset == 0 && t > start) {
            target = offsetSize;
            if (randomizer.yesOrNo()) {
                target *= -1;
            }
            step = (target - offset) * momentum;
        }
        offset += step;
        if (abs(offset) > abs(target)) {
            if (!invalid) {
                step *= -1;
            } else {
                target = randomizer.ratio() * offsetSize * 2 - offsetSize;
                step = (target - offset) * momentum;
            }
        }

        if (base->GetPoint(last_t, t, nextBasePoint)) {
            shapeNormal = {lastBasePoint.y - nextBasePoint.y, nextBasePoint.x - lastBasePoint.x};
            if (length(shapeNormal) > 0) {
                shapeNormal = normalize(shapeNormal);
                nextPoint = MovePoint(nextBasePoint, offset * shapeNormal);
            } else {
                nextPoint = nextBasePoint;
            }
            if (randomizer.ratio() >= skipRatio) {
                lines.emplace_back(lastPoint, nextPoint);
            }
        } else {
            nextPoint = nextBasePoint;
        }
        lastBasePoint = nextBasePoint;
        lastPoint = nextPoint;
        last_t = t;
    }

    return ImageLines(lines);
};

ImageLines DrawShape(ShapeDescriptor *shapeDescriptor) {
    float t = 0;
    float last_t = 0;
    vector<Line> lines;
    float2 lastPoint = shapeDescriptor->GetPoint(t);
    float2 nextPoint;
    while (t < 0.99) {
        t += 0.005;
        if (shapeDescriptor->GetPoint(last_t, t, nextPoint)) {
            lines.emplace_back(lastPoint, nextPoint);
        }
        last_t = t;
        lastPoint = nextPoint;
    }

    return ImageLines(lines);
};

ImageLines ComposeShapes(ShapeDescriptor *base, ShapeDescriptor *modifier, int multiplication) {
    float t = 0;
    ImageLines image;
    while (t < 0.99) {
        float2 point = base->GetPoint(t);
        auto part = DrawShape(modifier);
        auto translation = CreateTranslationMatrix(point - float2{0.5, 0.5});
        auto scale = CreateScalingMatrix(float2(1.5f / multiplication, 1.5f / multiplication));
        auto rotation = CreateRotationMatrix(ROTATIONS_ENABLED ? randomizer.ratio() : 0, float2{0.5, 0.5});
        part.Transform(MatrixMul(MatrixMul(translation, scale), rotation));

        image.Add(part);
        t += 1.f / multiplication / 1.5f;
    }

    return image;
};

void generateInvalidData(ofstream &trainingData,
                         int invalidDataCount, bool generate = false, bool saveImageLines = false) {
    for (int i = 0; i < invalidDataCount; ++i) {
        if (i % 500 == 0) {
            cout << "generating: " << to_string(i) << endl;
        }

        ImageLines image;
        if (randomizer.ratio() > 0.5f) {
            float offset = randomizer.ratio() + 0.5f;
            if (randomizer.yesOrNo()) {
                offset *= -1.f;
            }
            image = OffsetPoints(RandomShapeDescriptor(), offset, randomizer.ratio() * 0.3f,
                                 randomizer.ratio() * 0.015f + 0.03f, randomizer.ratio() * 0.3f, true);
        } else if (randomizer.ratio() > 0.1f) {
            vector<Line> lines;
            float2 start{randomizer.ratio(), randomizer.ratio()};
            float2 end;
            float2 previousStart;
            for (int k = 0; k < randomizer.nextExponential(); ++k) {
                float2 dir = start - previousStart;
                end = float2{randomizer.ratio(), randomizer.ratio()};
                if (randomizer.ratio() > 0.3) {
                    lines.emplace_back(start, (end + dir) * 0.3f);
                    previousStart = start;
                    start = end + dir;
                } else {
                    lines.emplace_back(start, end);
                    previousStart = start;
                    start = end;
                }
            }
            image.Add(ImageLines(lines));
        }

        auto rotation = CreateRotationMatrix(randomizer.ratio());
        image.Transform(rotation);
        image.Normalize();
        auto finalImage = DrawLines(image, IMAGE_SIDE_SIZE, LINE_DRAWING_STEP_SIZE);
        RandomizeData(finalImage, randomizer);

        if (saveImageLines) {
            std::string imageName = "generated/invalid" + std::to_string(i) + ".lines";
            image.SaveToFile(imageName);
        }
        if (generate) {
            std::string imageName = "generated/invalid" + std::to_string(i);
            finalImage.SaveToFile(imageName);
        }

        for (float grayScale : finalImage.Serialize()) {
            trainingData << grayScale << " ";
        }
        trainingData << std::endl;
        //output shape
        for (int j = 0; j < shapeDescriptors.size(); ++j) {
            trainingData << 0;
            trainingData << " ";
        }

        trainingData << std::endl;
    }
};

void Training::GenerateData(const std::string &filename, int validDataCount,
                            int invalidDataCount, bool generateImages, bool saveImageLines) {
    if (shapeDescriptors.empty()) {
        cout << "ERROR: no shape descriptors registered" << endl;
    }

    ofstream trainingData;
    trainingData.open(filename);
    int active = 0;
    string name;

    trainingData << validDataCount + invalidDataCount << " " << IMAGE_SIDE_SIZE * IMAGE_SIDE_SIZE << " "
                 << shapeDescriptors.size() << std::endl;


    for (int i = 0; i < validDataCount; i++) {
        name = "";
        if (i % 500 == 0) {
            cout << "generating: " << to_string(i) << endl;
        }
        active = active % (int) shapeDescriptors.size();
        auto *activeDescriptor = shapeDescriptors[active].get();
        auto *activeModifier = RandomShapeDescriptor();

        name += activeDescriptor->GetName();

        ImageLines image;
        if (randomizer.ratio() < COMPOSED_SHAPE_PROBABILITY && COMPOSED_SHAPES_ENABLED) {
            image = ComposeShapes(activeDescriptor, activeModifier, randomizer.next(6, 9));
            name += "_" + activeModifier->GetName();
        } else {
            float offset = (randomizer.ratio() * 0.2f - 0.1f);
            image = OffsetPoints(activeDescriptor, offset, randomizer.ratio(),
                                 randomizer.ratio() * 0.1f,
                                 0.5f * randomizer.ratio(), false);
            name += "_zero";
        }
        image.Normalize();

        if (EMBEDDED_SHAPES_ENABLED) {
            auto poi = activeDescriptor->GetPointsOfInterest();
            if (!poi.empty()) {
                for (float3 p : poi) {
                    if (randomizer.ratio() < FIRST_LEVEL_EMBEDDED_SHAPE_PROBABILITY) {
                        auto *innerShapeDescriptor = RandomShapeDescriptor();

                        name += "_" + innerShapeDescriptor->GetName();

                        ImageLines poiImage;
                        if (randomizer.ratio() > COMPOSED_SHAPE_PROBABILITY) {
                            poiImage = DrawShape(innerShapeDescriptor);
                            name += "_zero";
                        } else {
                            auto des = RandomShapeDescriptor();
                            poiImage = ComposeShapes(innerShapeDescriptor, des,
                                                     randomizer.next(6, 10));
                            name += "_" + des->GetName();
                        }
                        poiImage.Normalize();
                        float3 position = p;
                        auto translation = CreateTranslationMatrix(float2{position.x, position.y});
                        auto scaling = CreateScalingMatrix({position.z, position.z});
                        auto transformation = mul<float, 3, 3>(translation, scaling);
                        poiImage.Transform(transformation);
                        image.Add(poiImage);

                        poi = innerShapeDescriptor->GetPointsOfInterest();
                        if (!poi.empty()) {
                            for (float3 p2 : poi) {
                                if (randomizer.ratio() < SECOND_LEVEL_EMBEDDED_SHAPE_PROBABILITY) {
                                    innerShapeDescriptor = RandomShapeDescriptor();

                                    name += "_" + innerShapeDescriptor->GetName();

                                    ImageLines innerPoiImage;
                                    if (randomizer.ratio() > COMPOSED_SHAPE_PROBABILITY) {
                                        innerPoiImage = DrawShape(innerShapeDescriptor);
                                        name += "_zero";
                                    } else {
                                        auto des = RandomShapeDescriptor();
                                        innerPoiImage = ComposeShapes(innerShapeDescriptor, des,
                                                                      randomizer.next(8, 14));
                                        name += "_" + des->GetName();
                                    }
                                    innerPoiImage.Normalize();
                                    position = p2;
                                    translation = CreateTranslationMatrix(float2{position.x, position.y});
                                    scaling = CreateScalingMatrix({position.z * 0.8f, position.z * 0.8f});
                                    innerPoiImage.Transform(
                                            mul<float, 3, 3>(transformation, mul<float, 3, 3>(translation, scaling)));
                                    image.Add(innerPoiImage);
                                }
                            }
                        }
                    }
                }
            }
        }

        auto deformation = CreateScalingMatrix(
                float2{0.8f + randomizer.ratio() * 0.4f, 0.8f + randomizer.ratio() * 0.4f});
        image.Transform(deformation);
        auto rotation = CreateRotationMatrix(randomizer.ratio()*0.03f-0.015f,float2{0.5,0.5});
        image.Transform(rotation);

        image.Normalize();

        auto finalImage = DrawLines(image, IMAGE_SIDE_SIZE, LINE_DRAWING_STEP_SIZE);
        RandomizeData(finalImage, randomizer);
        if (saveImageLines) {
            std::string imageName =
                    "generated/" + activeDescriptor->GetName() + std::to_string(i) + "_" + name + ".lines";
            image.SaveToFile(imageName);
        }
        if (generateImages) {
            std::string imageName = "generated/" + activeDescriptor->GetName() + std::to_string(i);
            finalImage.SaveToFile(imageName);
        }

        for (float grayScale : finalImage.Serialize()) {
            trainingData << grayScale << " ";
        }
        trainingData << std::endl;
        //output shape
        for (int j = 0; j < shapeDescriptors.size(); ++j) {
            trainingData << (j == active ? 1 : 0);
            trainingData << " ";
        }
        trainingData << std::endl;
        active += 1;
    }

    generateInvalidData(trainingData, invalidDataCount, generateImages, saveImageLines);
    trainingData.close();
}

float Training::Train(const string &name, vector<unsigned int> networkStructure, bool generateData, float targetMSE, int dataSize) {
    if (shapeDescriptors.empty()) {
        cout << "ERROR: no shape descriptors registered for training" << endl;
        return 0;
    }

    inProgress = true;

    if (networkStructure.size() < 2){
        cout << "ERROR: the network configuration has to have at least two layers" << endl;
    }

    if (networkStructure[0] != IMAGE_SIDE_SIZE*IMAGE_SIDE_SIZE){
        cout << "ERROR: the first layer of the network should have size IMAGE_SIDE_SIZE*IMAGE_SIDE_SIZE" << endl;
        return 0;
    }

    if (networkStructure[networkStructure.size()-1] != shapeDescriptors.size()){
        cout << "ERROR: the last layer of the network should have the same size as the number of registered shape descriptors" << endl;
        return 0;
    }

    TrainingCase trainingCase(networkStructure);

    if (generateData) {
        int testDataSize = dataSize / 3;
        Training::GenerateData("test.data", testDataSize*3 / 4, testDataSize / 4, false);
        Training::GenerateData("training.data", dataSize *3/ 4, dataSize / 4, false);
    }

    auto *testData = fann_read_train_from_file("test.data");
    trainingCase.LoadData("training.data");
    float mse = trainingCase.Test(testData);
    float newMse = mse - 0.0001f;
    float error = 0.2f;
    trainingCase.SetError(error);

    while (newMse < mse && newMse > targetMSE) {
        mse = newMse;
        trainingCase.Train(100);
        newMse = trainingCase.Test(testData);
        error /= 2.f;
        trainingCase.SetError(error);
    }

    inProgress = false;

    trainingCase.Save(name+".net");

    return newMse;
};