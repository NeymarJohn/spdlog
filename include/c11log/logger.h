#pragma once

// Thread safe logger
// Has name, log level, vector of std::shared sink pointers and formatter
// Upon each log write the logger:
// 1. Checks if its log level is enough to log the message
// 2. Format the message using the formatter function
// 3. Pass the formatted message to it sinks to performa the actual logging

#include<vector>
#include<memory>
#include<atomic>
#include <sstream>

#include "sinks/base_sink.h"
#include "common.h"
#include "pattern_formatter.h"

namespace c11log
{

namespace details
{
class line_logger;
}

class logger
{
public:
    using sink_ptr = std::shared_ptr<sinks::sink>;
    using sinks_vector_t = std::vector<sink_ptr>;
    using sinks_init_list = std::initializer_list<sink_ptr>;
    using formatter_ptr = std::unique_ptr<formatter>;

    logger(const std::string& name, sinks_init_list, formatter_ptr = nullptr);
    template<class It>
    logger(const std::string& name, It begin, It end, formatter_ptr = nullptr);
    logger(const std::string& name, sink_ptr, formatter_ptr = nullptr);
    ~logger() = default;

    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

    void level(level::level_enum);
    level::level_enum level() const;

    const std::string& name() const;
    bool should_log(level::level_enum) const;


    template <typename... Args> details::line_logger log(level::level_enum lvl, const Args&... args);
    template <typename... Args> details::line_logger trace(const Args&... args);
    template <typename... Args> details::line_logger debug(const Args&... args);
    template <typename... Args> details::line_logger info(const Args&... args);
    template <typename... Args> details::line_logger warn(const Args&... args);
    template <typename... Args> details::line_logger error(const Args&... args);
    template <typename... Args> details::line_logger critical(const Args&... args);




private:
    friend details::line_logger;
    std::string _name;
    formatter_ptr _formatter;
    sinks_vector_t _sinks;
    std::atomic_int _level;

    void _variadic_log(details::line_logger& l);
    template <typename First, typename... Rest>
    void _variadic_log(details::line_logger&l, const First& first, const Rest&... rest);
    void _log_msg(details::log_msg& msg);


};
}

//
// trace & debug macros
//
#ifdef FFLOG_ENABLE_TRACE
#define FFLOG_TRACE(logger, ...) logger->log(c11log::level::TRACE, __FILE__, " #", __LINE__,": " __VA_ARGS__)
#else
#define FFLOG_TRACE(logger, ...) {}
#endif

#ifdef FFLOG_ENABLE_DEBUG
#define FFLOG_DEBUG(logger, ...) logger->log(c11log::level::DEBUG, __VA_ARGS__)
#else
#define FFLOG_DEBUG(logger, ...) {}
#endif

//
// Logger implementation
//

#include "details/line_logger.h"


inline c11log::logger::logger(const std::string& name, sinks_init_list sinks_list, formatter_ptr f) :
    _name(name),
    _formatter(std::move(f)),
    _sinks(sinks_list)
{
    //Seems that vs2013 doesn't support std::atomic member initialization yet
    _level = level::INFO;
    if(!_formatter)
        _formatter = std::make_unique<pattern_formatter>("%t");

}

template<class It>
inline c11log::logger::logger(const std::string& name, It begin, It end, formatter_ptr f):
    _name(name),
    _formatter(std::move(f)),
    _sinks(begin, end)
{

}

inline c11log::logger::logger(const std::string& name, sink_ptr sink, formatter_ptr f) :
    logger(name, {sink}, std::move(f)) {}


template <typename... Args>
inline c11log::details::line_logger c11log::logger::log(level::level_enum lvl, const Args&... args) {
    details::line_logger l(this, lvl, true);
    _variadic_log(l, args...);
    return l;
}


template <typename... Args>
inline c11log::details::line_logger c11log::logger::trace(const Args&... args) {
    return log(level::TRACE, args...);
}

template <typename... Args>
inline c11log::details::line_logger c11log::logger::debug(const Args&... args) {
    return log(level::DEBUG, args...);
}

template <typename... Args>
inline c11log::details::line_logger c11log::logger::info(const Args&... args) {
    return log(level::INFO, args...);
}


template <typename... Args>
inline c11log::details::line_logger c11log::logger::warn(const Args&... args) {
    return log(level::WARNING, args...);
}

template <typename... Args>
inline c11log::details::line_logger c11log::logger::critical(const Args&... args) {
    return log(level::CRITICAL, args...);
}



inline const std::string& c11log::logger::name() const
{
    return _name;
}

inline void c11log::logger::level(c11log::level::level_enum level)
{
    _level.store(level);
}

inline c11log::level::level_enum c11log::logger::level() const
{
    return static_cast<c11log::level::level_enum>(_level.load());
}

inline bool c11log::logger::should_log(c11log::level::level_enum level) const
{
    return level >= _level.load();
}


inline void c11log::logger::_variadic_log(c11log::details::line_logger& l) {}

template <typename First, typename... Rest>
void c11log::logger::_variadic_log(c11log::details::line_logger& l, const First& first, const Rest&... rest)
{
    l.write(first);
    _variadic_log(l, rest...);
}

inline void c11log::logger::_log_msg(details::log_msg& msg)
{
    _formatter->format(msg);
    for (auto &sink : _sinks)
        sink->log(msg);
}


