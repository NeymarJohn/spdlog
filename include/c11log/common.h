#pragma once

#include<initializer_list>
#include<memory>
#include<chrono>

namespace c11log
{
class formatter;
namespace sinks {
class sink;
}

// Common types across the lib
using log_clock = std::chrono::system_clock;
using sink_ptr = std::shared_ptr < sinks::sink > ;
using sinks_init_list = std::initializer_list < sink_ptr > ;
using formatter_ptr = std::shared_ptr<c11log::formatter>;

//Log level enum
namespace level
{
typedef enum
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERR,
    CRITICAL,
    NONE = 99
} level_enum;

static const char* level_names[] { "trace", "debug", "info", "warning", "error", "critical", "fatal" };
inline const char* to_str(c11log::level::level_enum l)
{
    return level_names[l];
}
} //level
} //c11log
