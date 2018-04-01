#pragma once

#include <cstdlib>
#include <random>
#include <mutex>


class Randomizer {
private:
    std::default_random_engine Generator;
    std::uniform_real_distribution<double> Real;
    std::uniform_int_distribution<int> Randmax;
    std::exponential_distribution<float> Exp;
    //std::mutex Lock;
public:
    Randomizer() : Real(0, 1), Exp(1 / 4.0f), Randmax(0, RAND_MAX) {
        Generator.seed(static_cast<unsigned long>(time(0)));
    }


    bool yesOrNo() {
        //std::lock_guard<std::mutex> lg(Lock);
        return ((Real(Generator)) > 0.5);
    }


    float ratio() {
        //std::lock_guard<std::mutex> lg(Lock);
        return (float) (Real(Generator));
    }

    int next(int max) {
        //std::lock_guard<std::mutex> lg(Lock);
        if (max == 0) {
            return 0;
        }
        int res = Randmax(Generator) % max;
        return res;
    }

    int next(int min, int max) {
        int range = max - min;
        return next(range) + min;
    }

    float nextExponential() {
        return Exp(Generator);
    }
};
