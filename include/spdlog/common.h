#pragma once

#include<initializer_list>
#include<chrono>

namespace spdlog
{
class formatter;
namespace sinks
{
class sink;
}

// Common types across the lib
using log_clock = std::chrono::system_clock;
using sink_ptr = std::shared_ptr < sinks::sink > ;
using sinks_init_list = std::initializer_list < sink_ptr > ;
using formatter_ptr = std::shared_ptr<spdlog::formatter>;

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
    ALWAYS,
    OFF
} level_enum;

static const char* level_names[] { "trace", "debug", "info", "warning", "error", "critical", "", ""};
inline const char* to_str(spdlog::level::level_enum l)
{
    return level_names[l];
}
} //level

//
// Log exception
//
class spdlog_ex : public std::exception
{
public:
    spdlog_ex(const std::string& msg) :_msg(msg) {};
    const char* what() const throw() override
    {
        return _msg.c_str();
    }
private:
    std::string _msg;

};

} //spdlog
