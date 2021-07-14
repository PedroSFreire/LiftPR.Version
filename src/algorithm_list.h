#pragma once

#include <string>
#include <vector>

enum class Algorithm {VCM, PT, BDPT, LT, PPM };

class AlgorithmList {
public:
    static const std::vector<std::pair<std::string, Algorithm>> all_algorithms;
};

