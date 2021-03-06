#pragma once

#include <chrono>
#include <map>
#include <string>

class Profiler {
public:
    enum class Id { SceneLoad, Trace, Display, Denoise, Other };

    Profiler(Id id);
    Profiler(std::string id);
    ~Profiler();

    static auto getDuration(Id id) -> float;
    static void reset(Id id);

private:
    std::chrono::time_point<std::chrono::steady_clock> start_, end_;
    std::chrono::duration<float> duration_;
    std::string message_;
    Id id_;
    static std::map<Id, float> timers_;
};
