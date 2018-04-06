
#include <ctime>
#include "Training.h"

void ManualTraining() {
    using namespace Training;
    TrainingCase network("network.net",
                         vector<unsigned int>{NETWORK_INPUT_SIZE, 128, 8, NETWORK_OUTPUT_SIZE});
    string dataName = "data.data";
    string networkName = "network.net";

    int validDataCount;
    int invalidDataCount;
    int epochCount;
    int repetitions;
    float learningRate;
    float learningMomentum;

    while (true) {
        char command;
        cout << "awaiting command" << endl;
        cin >> command;
        switch (command) {
            case 'g':
                cout << "valid:" << endl;
                cin >> validDataCount;
                cout << " invalid:" << endl;
                cin >> invalidDataCount;

                Training::generateData(dataName, validDataCount, invalidDataCount);
                network.LoadData(dataName);
                cout << "data generated" << endl;
                continue;
            case 'e':
                cout << "epochCount:" << endl;
                cin >> epochCount;
                network.Train(epochCount);
                continue;
            case 's':
                network.Save(networkName);
                cout << "saved" << endl;
                continue;
            case 't':
                network.Test(cout);
                continue;
            case 'm':
                cout << "learning rate (default 0.7): " << endl;
                cin >> learningRate;
                cout << "learning momentum (default 0): " << endl;
                cin >> learningMomentum;
                network.SetLearningParams(learningRate, learningMomentum);
                continue;
            case 'q':
                network.Save(networkName);
                return;
            case 'c':
                cout << "valid:" << endl;
                cin >> validDataCount;
                cout << " invalid:" << endl;
                cin >> invalidDataCount;
                cout << "epochCount:" << endl;
                cin >> epochCount;
                cout << "repetitions:" << endl;
                cin >> repetitions;
                for (int i = 0; i < repetitions; ++i) {
                    auto t = std::time(nullptr);
                    cout << "time: " << std::asctime(std::localtime(&t)) << endl;
                    cout << "repetition: " << i << " from " << repetitions << endl;
                    Training::generateData(dataName, validDataCount, invalidDataCount);
                    network.LoadData(dataName);
                    cout << "data generated" << endl;
                    network.Train(epochCount);
                }
                continue;
            case 'Q':
                return;
            default:
                cout << "invalid command" << endl;
                cout << "g generate" << endl;
                cout << "c complex" << endl;
                cout << "m learning params" << endl;
                cout << "e train" << endl;
                cout << "t test" << endl;
                cout << "s save" << endl;
                cout << "q quit" << endl;
                cout << "Q quit without save" << endl;
                continue;

        }
    }
};

void BatchTraining() {
    using namespace Training;

    int data = 10000;
    int epochsCount = 100;
    vector<int> batches{1, 5, 10};
    vector<unsigned int> structure{NETWORK_INPUT_SIZE, 64, 64, NETWORK_OUTPUT_SIZE};

    for (int i = 0; i < batches.size(); ++i) {
        string name = "networkBatch" + to_string(batches[i]);
        string networkName = name + ".net";
        ofstream output{name};

        output << "data size: " << to_string(data) << endl;
        output << "epochsCount: " << to_string(epochsCount) << endl;
        output << "structure: ";
        for (auto s: structure) {
            output << "\t" << to_string(s);
        }
        output << endl;

        TrainingCase c(networkName, structure);
        c.BatchTrain(data / batches[i], data / (batches[i] * 2), epochsCount / batches[i], batches[i]);
        c.Test(output);

        output.close();
    }
};

void StructureTraining() {
    using namespace Training;

    int validData = 20000;
    int invalidData = 10000;
    int epochsCount = 20;
    int batchCount = 10;

    TrainingCase trainingCase16("16,16.net", vector<unsigned int>{NETWORK_INPUT_SIZE, 16, 16, NETWORK_OUTPUT_SIZE});
    TrainingCase trainingCase32("32,32.net", vector<unsigned int>{NETWORK_INPUT_SIZE, 32, 32, NETWORK_OUTPUT_SIZE});
    TrainingCase trainingCase64("64,64.net", vector<unsigned int>{NETWORK_INPUT_SIZE, 64, 64, NETWORK_OUTPUT_SIZE});
    TrainingCase trainingCase128("128,128.net",
                                 vector<unsigned int>{NETWORK_INPUT_SIZE, 128, 128, NETWORK_OUTPUT_SIZE});

    for (int batch = 0; batch < batchCount; ++batch) {
        cout << endl << "BATCH: " << to_string(batch) << endl;

        string dataFile = "tmp.data";
        generateData(dataFile, validData, invalidData);
        trainingCase16.LoadData(dataFile);
        trainingCase32.LoadData(dataFile);
        trainingCase64.LoadData(dataFile);
        trainingCase128.LoadData(dataFile);

        trainingCase16.Train(epochsCount);
        trainingCase32.Train(epochsCount);
        trainingCase64.Train(epochsCount);
        trainingCase128.Train(epochsCount);
    }

    ofstream output16(trainingCase16.GetName() + ".output");
    ofstream output32(trainingCase32.GetName() + ".output");
    ofstream output64(trainingCase64.GetName() + ".output");
    ofstream output128(trainingCase128.GetName() + ".output");

    output16 << "data size: " << to_string((validData + invalidData) * batchCount) << endl;
    output16 << "epochsCount: " << to_string(batchCount * epochsCount) << endl;
    output16 << "structure: " + trainingCase16.GetName();
    output16 << endl;

    output32 << "data size: " << to_string((validData + invalidData) * batchCount) << endl;
    output32 << "epochsCount: " << to_string(batchCount * epochsCount) << endl;
    output32 << "structure: " + trainingCase32.GetName();
    output32 << endl;

    output64 << "data size: " << to_string((validData + invalidData) * batchCount) << endl;
    output64 << "epochsCount: " << to_string(batchCount * epochsCount) << endl;
    output64 << "structure: " + trainingCase64.GetName();
    output64 << endl;

    output128 << "data size: " << to_string((validData + invalidData) * batchCount) << endl;
    output128 << "epochsCount: " << to_string(batchCount * epochsCount) << endl;
    output128 << "structure: " + trainingCase128.GetName();

    output128 << endl;

    trainingCase16.Test(output16);
    trainingCase32.Test(output32);
    trainingCase64.Test(output64);
    trainingCase128.Test(output128);

    output16.close();
    output32.close();
    output64.close();
    output128.close();
};

int main() {

//    StructureTraining();

    ManualTraining();

//    Training::generateData("data",400,400,true);

    return 0;
}
