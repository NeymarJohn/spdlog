/*************************************************************************/
/* spdlog - an extremely fast and easy to use c++11 logging library.     */
/* Copyright (c) 2014 Gabi Melman.                                       */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#pragma once

// Thread safe logger
// Has name, log level, vector of std::shared sink pointers and formatter
// Upon each log write the logger:
// 1. Checks if its log level is enough to log the message
// 2. Format the message using the formatter function
// 3. Pass the formatted message to its sinks to performa the actual logging

#include<vector>
#include<memory>
#include "sinks/base_sink.h"
#include "common.h"

namespace spdlog
{

namespace details
{
class line_logger;
}

class logger
{
public:
    logger(const std::string& logger_name, sink_ptr single_sink);
    logger(const std::string& name, sinks_init_list);
    template<class It>
    logger(const std::string& name, const It& begin, const It& end);

    virtual ~logger();
    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

    void set_level(level::level_enum);
    level::level_enum level() const;

    const std::string& name() const;
    bool should_log(level::level_enum) const;

    //Stop logging
    void stop();

    template <typename... Args> details::line_logger log(const std::string& fmt, const Args&... args);
    template <typename... Args> details::line_logger trace(const std::string& fmt, const Args&... args);
    template <typename... Args> details::line_logger debug(const std::string& fmt, const Args&... args);
    template <typename... Args> details::line_logger info(const std::string& fmt, const Args&... args);
    template <typename... Args> details::line_logger notice(const std::string& fmt, const Args&... args);
    template <typename... Args> details::line_logger warn(const std::string& fmt, const Args&... args);
    template <typename... Args> details::line_logger error(const std::string& fmt, const Args&... args);
    template <typename... Args> details::line_logger critical(const std::string& fmt, const Args&... args);
    template <typename... Args> details::line_logger alert(const std::string& fmt, const Args&... args);
    template <typename... Args> details::line_logger emerg(const std::string& fmt, const Args&... args);

    //API to support logger.info() << ".." calls

    details::line_logger log();
    details::line_logger trace();
    details::line_logger debug();
    details::line_logger info();
    details::line_logger notice();
    details::line_logger warn();
    details::line_logger error();
    details::line_logger critical();
    details::line_logger alert();
    details::line_logger emerg();


    void set_pattern(const std::string&);
    void set_formatter(formatter_ptr);


protected:
    virtual void _log_msg(details::log_msg&);
    virtual void _set_pattern(const std::string&);
    virtual void _set_formatter(formatter_ptr);
    virtual void _stop();
    details::line_logger _log(level::level_enum lvl);
    template <typename... Args> details::line_logger _log(level::level_enum lvl, const std::string& fmt, const Args&... args);


    friend details::line_logger;
    std::string _name;
    std::vector<sink_ptr> _sinks;
    formatter_ptr _formatter;
    std::atomic_int _level;

};
}

#include "./details/logger_impl.h"
