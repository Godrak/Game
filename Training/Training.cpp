#include <glob.h>
#include "Training.h"
#include "../ImageAnalyzer/Params.h"
#include "../ImageAnalyzer/LineDrawer.h"


using namespace ShapeDescriptors;
using namespace ImageAnalyzer;
using namespace LineDrawer;

std::string DetermineCorrectShape(std::string fileName) {
    switch (fileName[5]) {
        //todo fix
        case 't':
        case 'T':
            return ShapeToString(TRIANGLE);
        case 'r':
        case 'R':
            return ShapeToString(RECTANGLE);
        case 'c':
        case 'C':
            return ShapeToString(CIRCLE);
        case 'l':
        case 'L':
            return ShapeToString(WATER);
        case 'w':
        case 'W':
            return ShapeToString(WAVY);
        case 'u':
        case 'U':
        case 'i':
            return ShapeToString(UNKNOWN);
        default:
            return "#ERROR:INVALID_FILE_NAME";
    }
};

const ShapeDescriptor *RandomShapeDescriptor() {
    return SHAPE_DESCRIPTORS[RANDOMIZER.next(SHAPE_DESCRIPTORS.size())];
}

ImageLines OffsetPoints(const ShapeDescriptor *base, float offsetSize, float start, float momentum) {
    float t = 0;
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
            if (RANDOMIZER.yesOrNo()) {
                target *= -1;
            }
            step = (target - offset) * momentum;
        }
        offset += step;
        if (abs(offset) > abs(target)) {
            step = (target - offset) * momentum;
        }

        nextBasePoint = base->GetPoint(t);
        shapeNormal = {lastBasePoint.y - nextBasePoint.y, nextBasePoint.x - lastBasePoint.x};
        if (length(shapeNormal) > 0) {
            shapeNormal = normalize(shapeNormal);
            nextPoint = MovePoint(nextBasePoint, offset * shapeNormal);
        } else {
            nextPoint = nextBasePoint;
        }
        lines.emplace_back(lastPoint, nextPoint);
        lastBasePoint = nextBasePoint;
        lastPoint = nextPoint;
    }

    return ImageLines(lines);
};

Line CreateRandomLine() {
    auto point = float2(RANDOMIZER.ratio() * 0.9f, RANDOMIZER.ratio() * 0.9f);
    return Line{
            point, point + 0.05f
    };
}

ImageLines DrawShape(const ShapeDescriptor *shapeDescriptor) {
    float t = 0;
    vector<Line> lines;
    float2 lastPoint = shapeDescriptor->GetPoint(t);
    while (t < 0.99) {
        t += 0.005;
        float2 nextPoint = shapeDescriptor->GetPoint(t);
        lines.emplace_back(lastPoint, nextPoint);
        lastPoint = nextPoint;
    }

    return ImageLines(lines);
};

ImageLines ComposeShapes(const ShapeDescriptor *base, const ShapeDescriptor *modifier, int multiplication) {
    float t = 0;
    ImageLines image;
    while (t < 0.99) {
        float2 point = base->GetPoint(t);
        auto part = DrawShape(modifier);
        auto translation = CreateTranslationMatrix(point - float2{0.5, 0.5});
        auto scale = CreateScalingMatrix(float2(1.5f / multiplication, 1.5f / multiplication));
        part.Transform(mul<float, 3, 3>(translation, scale));

        image.Add(part);
        t += 1.f / multiplication / 1.5f;
    }

    return image;
};

void generateInvalidData(ofstream &trainingData,
                         int invalidDataCount, bool generate = false) {
    for (int i = 0; i < invalidDataCount; ++i) {
        if (i % 500 == 0) {
            cout << "generating: " << to_string(i) << endl;
        }

        ImageLines image;
        if (RANDOMIZER.ratio() > 0.2) {
            float offset = RANDOMIZER.ratio() + 1.f;
            if (RANDOMIZER.yesOrNo()) {
                offset *= -1.f;
            }
            image = OffsetPoints(RandomShapeDescriptor(), offset, RANDOMIZER.ratio() * 0.8f,
                                 RANDOMIZER.ratio() * 0.1f + 0.05f);
        }

        if (RANDOMIZER.ratio() > 0.5f) {
            vector<Line> lines;
            float2 start{RANDOMIZER.ratio(), RANDOMIZER.ratio()};
            float2 end;
            for (int k = 0; k < RANDOMIZER.next(4, 16); ++k) {
                end = float2{RANDOMIZER.ratio(), RANDOMIZER.ratio()};
                lines.emplace_back(start, end);
                start = end;
            }
        }

        auto rotation = CreateRotationMatrix(RANDOMIZER.ratio());
        image.Transform(rotation);
        image.Normalize();
        auto finalImage = DrawLines(image);
        RandomizeData(finalImage);

        if (generate) {
            std::string imageName = "generated/invalid" + std::to_string(i);
            finalImage.SaveToFile(imageName);
        }

        for (float grayScale : finalImage.Serialize()) {
            trainingData << grayScale << " ";
        }
        trainingData << std::endl;
        //output shape
        for (int j = 0; j < NETWORK_OUTPUT_SIZE; ++j) {
            trainingData << 0;
            trainingData << " ";
        }

        trainingData << std::endl;
    }
};

void Training::generateData(const std::string &filename, int validDataCount,
                            int invalidDataCount, bool generateImages) {
    ofstream trainingData;
    trainingData.open(filename);
    int active = 0;

    trainingData << validDataCount + invalidDataCount << " " << NETWORK_INPUT_SIZE << " "
                 << NETWORK_OUTPUT_SIZE << std::endl;


    for (int i = 0; i < validDataCount; i++) {
        if (i % 500 == 0) {
            cout << "generating: " << to_string(i) << endl;
        }
        active = active % (int) SHAPE_DESCRIPTORS.size();
        const auto *activeDescriptor = SHAPE_DESCRIPTORS[active];
        const auto *activeModifier = RandomShapeDescriptor();

        ImageLines image;
        if (RANDOMIZER.ratio() > 0.5f)
            image = ComposeShapes(activeDescriptor, activeModifier, RANDOMIZER.next(8, 14));
        else {
            float offset = (RANDOMIZER.ratio() * 0.4f - 0.2f);
            image = OffsetPoints(activeDescriptor, offset, RANDOMIZER.ratio()*0.8f, RANDOMIZER.ratio() * 0.01f);
        }
        image.Normalize();

        auto poi = activeDescriptor->GetPointsOfInterest();
        if (!poi.empty() && RANDOMIZER.ratio() > 0.5) {
            auto poiImage = DrawShape(RandomShapeDescriptor());
            float3 position = poi[RANDOMIZER.next(poi.size())];
            auto translation = CreateTranslationMatrix(float2{position.x, position.y});
            auto scaling = CreateScalingMatrix({position.z * 0.8f, position.z * 0.8f});
            poiImage.Transform(mul<float, 3, 3>(translation, scaling));
            image.Add(poiImage);
        }

        auto deformation = CreateScalingMatrix(
                float2{0.8f + RANDOMIZER.ratio() * 0.4f, 0.8f + RANDOMIZER.ratio() * 0.4f});
        image.Transform(deformation);

        image.Normalize();

        vector<Line> randomLines;
        for (int k = 0; k < RANDOMIZER.next(0, 5); ++k) {
            randomLines.push_back(CreateRandomLine());
        }
        image.Add(ImageLines(randomLines));


        auto finalImage = DrawLines(image);
        RandomizeData(finalImage);
        if (generateImages) {
            std::string imageName = "generated/" + ShapeToString(Shape(active)) + std::to_string(i);
            finalImage.SaveToFile(imageName);
        }

        for (float grayScale : finalImage.Serialize()) {
            trainingData << grayScale << " ";
        }
        trainingData << std::endl;
        //output shape
        for (int j = 0; j < NETWORK_OUTPUT_SIZE; ++j) {
            trainingData << (j == active ? 1 : 0);
            trainingData << " ";
        }
        trainingData << std::endl;
        active += 1;
    }

    generateInvalidData(trainingData, invalidDataCount, generateImages);
    trainingData.close();
};


Training::TrainingCase::TrainingCase(string networkName, vector<unsigned int> networkStructure)
        : networkName(networkName), networkStructure(networkStructure) {

    ann = fann_create_standard_array(networkStructure.size(), networkStructure.data());

    fann_set_activation_function_hidden(ann, FANN_ELLIOT);
    fann_set_training_algorithm(ann, FANN_TRAIN_INCREMENTAL);
    fann_set_train_stop_function(ann, FANN_STOPFUNC_BIT);
}

void Training::TrainingCase::Train(int epochCount) {
    fann_train_on_data(ann, data, epochCount, 1, 0.f);
}

void Training::TrainingCase::LoadData(const string &filename) {
    if (data != nullptr) {
        fann_destroy_train(data);
    }
    data = fann_read_train_from_file(filename.c_str());
}

void Training::TrainingCase::Test(ostream &output) {
    vector<string> testFiles;
    glob_t globResult;
    glob("test/*", GLOB_TILDE, NULL, &globResult);
    for (int i = 0; i < globResult.gl_pathc; ++i) {
        testFiles.emplace_back(globResult.gl_pathv[i]);
    }
    vector<GrayScaleImage> testData;

    for (auto &testFile : testFiles) {
        GrayScaleImage test(testFile);

        auto *data = fann_run(ann, test.Serialize().data());
        vector<float> result(fann_run(ann, test.Serialize().data()), data + NETWORK_OUTPUT_SIZE);
        output << testFile << "\t" << DetermineCorrectShape(testFile) << "\t";
        for (int j = 0; j < result.size(); ++j) {
            output << (int) (result[j] * 100) << "\t";
        }
        output << endl;
    }
}

void Training::TrainingCase::Save(const string &filename) {
    fann_save(ann, filename.c_str());
}

void
Training::TrainingCase::BatchTrain(uint validDataCount, uint invalidDataCount, uint epochsPerBatch, uint batchCount) {
    for (int i = 0; i < batchCount; ++i) {
        cout << "batch: " << i + 1 << " from " << batchCount << endl;
        generateData("batch.data", validDataCount, invalidDataCount);
        LoadData("batch.data");
        cout << "data generated" << endl;
        Train(epochsPerBatch);
    }
}

void Training::TrainingCase::SetLearningParams(float learningRate, float learningMomentum) {
    fann_set_learning_rate(ann, learningRate);
    fann_set_learning_momentum(ann, learningMomentum);

}


