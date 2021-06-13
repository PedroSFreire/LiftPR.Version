#pragma once

#include <string>
#include <vector>

enum class Algorithm { PT, BDPT, LT, VCM };

class AlgorithmList {
public:
    static const std::vector<std::pair<std::string, Algorithm>> all_algorithms;
};

