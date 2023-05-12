#pragma once

#include "../common.h"
#include "fast_oss.h"

namespace c11log
{
namespace details
{
struct log_msg
{
    log_msg() = default;
    log_msg(level::level_enum l):
        logger_name(),
        level(l),
        time(),
        tm_time(),
        raw(),
        formatted() {}

    log_msg(const log_msg& other):
        logger_name(other.logger_name),
        level(other.level),
        time(other.time),
        tm_time(other.tm_time),
        raw(other.raw),
        formatted(other.formatted) {}

    log_msg(log_msg&& other)
    {
        swap(*this, other);
    }

    void swap(log_msg& l, log_msg& r)
    {
        using std::swap;
        swap(l.logger_name, r.logger_name);
        swap(l.level, r.level);
        swap(l.time, r.time);
        swap(l.tm_time, r.tm_time);
        //swap(l.raw, r.raw);
        //swap(l.formatted, r.formatted);
    }


    log_msg& operator=(log_msg other)
    {
        swap(*this, other);
        return *this;
    }


    void clear()
    {
        raw.clear();
        formatted.clear();
    }

    std::string logger_name;
    level::level_enum level;
    log_clock::time_point time;
    std::tm tm_time;
    fast_oss raw;
    fast_oss formatted;


};
}
}
