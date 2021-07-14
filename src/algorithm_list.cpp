#include "algorithm_list.h"

const std::vector<std::pair<std::string, Algorithm>> AlgorithmList::all_algorithms = {
    {"Vertex Connection and Merging", Algorithm::VCM},
    {"Path Tracing", Algorithm::PT},
    {"Bidirectional Path Tracing", Algorithm::BDPT},
    {"Light Tracing", Algorithm::LT},
    {"Vertex Merging", Algorithm::PPM},
    
};
