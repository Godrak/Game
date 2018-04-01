#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <memory>
#include <floatfann.h>


class NeuralNetwork {
public:
    explicit NeuralNetwork(const char *file) {
        ann = fann_create_from_file(file);
    }

    std::vector<float> Calculate(const std::vector<float> &inputs) const {
        fann_type input[inputs.size()];
        for (int i = 0; i < inputs.size(); ++i) {
            input[i] = inputs[i];
        }

        fann_type *calc_out = fann_run(ann, input);
        std::vector<float> result;
        for (int j = 0; j < ann->num_output; ++j) {
            result.push_back(calc_out[j]);
        }
        return result;
    }

    virtual ~NeuralNetwork() {
        fann_safe_free(ann);
    }

private:
    struct fann *ann;
};

