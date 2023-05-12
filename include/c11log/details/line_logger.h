#pragma once

#include <sstream>
#include "../common_types.h"
#include "../logger.h"


// line_logger class.
// aggregates single log line (on the stack if possibe) and calls the logger upon destruction

namespace c11log
{
//class logger;
namespace details
{

class line_logger
{
public:
    line_logger(logger* callback_logger, level::level_enum msg_level, bool enabled):
        _callback_logger(callback_logger),
        _log_msg(msg_level),
        _oss(),
        _enabled(enabled),
        _empty(true)
    {
        if(enabled)
        {
            _log_msg.time = log_clock::now();
        }
    }

    // No copy intended. Only move
    line_logger(const line_logger& other) = delete;
    line_logger& operator=(const line_logger&) = delete;
    line_logger& operator=(line_logger&&) = delete;


    line_logger(line_logger&& other) :
        _callback_logger(other._callback_logger),
        _log_msg(std::move(other._log_msg)),
        _oss(std::move(other._oss.str())),
        _enabled(other._enabled),
        _empty(other._empty)
    {
        other.disable();
    }



    ~line_logger()
    {
        //only if enabled and not empty
        if (_enabled && !_empty)
        {
            _log_msg.raw = _oss.str();
            _callback_logger->_log_it(_log_msg);
        }
    }

    template<typename T>
    void write(const T& what)
    {
        if (_enabled)
        {
            _oss << what;
            _empty = false;
        }
    }

    template<typename T>
    line_logger& operator<<(const T& what)
    {
        write(what);
        return *this;
    }

    void disable()
    {
        _enabled = false;
    }



private:
    logger* _callback_logger;
    log_msg _log_msg;
    //details::stack_oss _oss;
    std::ostringstream _oss;
    bool _enabled;
    bool _empty;
};
} //Namespace details
} // Namespace c11log
