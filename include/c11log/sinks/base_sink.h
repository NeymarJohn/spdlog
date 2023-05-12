#pragma once

#include<string>
#include<atomic>

#include "../formatters/formatters.h"
#include "../level.h"

namespace c11log {
namespace sinks {
class base_sink {
public:
    base_sink() = default;
    base_sink(level::level_enum l):_level(l)
    {};
    virtual ~base_sink() = default;

    base_sink(const base_sink&) = delete;
    base_sink& operator=(const base_sink&) = delete;

    void log(const std::string &msg, level::level_enum level)
    {
        if (level >= _level) {
            sink_it_(msg);
        }
    };

    void set_level(level::level_enum level)
    {
        _level = level;
    }

protected:
    virtual void sink_it_(const std::string& msg) = 0;
    std::atomic<int> _level = level::INFO;
};

class null_sink:public base_sink {
protected:
    void sink_it_(const std::string& msg) override
    {}
};
}
}