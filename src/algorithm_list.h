#pragma once

#include <string>
#include <vector>

enum class Algorithm {PT, BDPT, VM, VC, VCM };

class AlgorithmList {
public:
    static const std::vector<std::pair<std::string, Algorithm>> all_algorithms;
};

