#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <memory>
#include <floatfann.h>
#include <iostream>


class NeuralNetwork {
public:
    NeuralNetwork() = default;

    void Load(const char *file) {
        if (loaded) Unload();
        ann = fann_create_from_file(file);
        loaded = true;
    }

    std::vector<float> Calculate(std::vector<float> &inputs) const {
        if (!loaded) {
            std::cout << "ERROR: NO NEURAL NETWORK LOADED" << std::endl;
        }

        fann_type *calc_out = fann_run(ann, inputs.data());
        std::vector<float> result;
        for (int j = 0; j < ann->num_output; ++j) {
            result.push_back(calc_out[j]);
        }
        return result;
    }

private:
    struct fann *ann{};
    bool loaded = false;

    void Unload() {
        if (loaded) fann_safe_free(ann);
    }
};

