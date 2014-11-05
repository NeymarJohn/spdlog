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
// Loggers registy of unique name->logger pointer
// An attempt to create a logger with an alreasy existing name will be ignored
// If user requests a non existing logger, nullptr will be returned
// This class is thread safe

#include <string>
#include <mutex>
#include <unordered_map>

#include "../logger.h"
#include "../common.h"

namespace spdlog
{
namespace details
{

class registry
{
public:
    std::shared_ptr<logger> get(const std::string& logger_name)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto found = _loggers.find(logger_name);
        return found == _loggers.end() ? nullptr : found->second;
    }

    template<class It>
    std::shared_ptr<logger> create(const std::string& logger_name, const It& sinks_begin, const It& sinks_end)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        //If already exists, just return it
        auto found = _loggers.find(logger_name);
        if (found != _loggers.end())
            return found->second;

        auto new_logger = std::make_shared<logger>(logger_name, sinks_begin, sinks_end);
        new_logger->set_formatter(_formatter);
        new_logger->set_level(_level);
        _loggers[logger_name] = new_logger;
        return new_logger;
    }


    std::shared_ptr<logger> create(const std::string& logger_name, sinks_init_list sinks)
    {
        return create(logger_name, sinks.begin(), sinks.end());
    }

    std::shared_ptr<logger> create(const std::string& logger_name, sink_ptr sink)
    {
        return create(logger_name, { sink });
    }


    void formatter(formatter_ptr f)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _formatter = f;
        for (auto& l : _loggers)
            l.second->set_formatter(_formatter);
    }


    void set_pattern(const std::string& pattern)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _formatter = std::make_shared<pattern_formatter>(pattern);
        for (auto& l : _loggers)
            l.second->set_formatter(_formatter);

    }

    void set_level(level::level_enum log_level)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        for (auto& l : _loggers)
            l.second->set_level(log_level);

    }

    void stop_all()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _level = level::OFF;
        for (auto& l : _loggers)
            l.second->stop();
    }


    static registry& instance()
    {
        static registry s_instance;
        return s_instance;
    }

private:
    registry() = default;
    registry(const registry&) = delete;
    registry& operator=(const registry&) = delete;
    std::mutex _mutex;
    std::unordered_map <std::string, std::shared_ptr<logger>> _loggers;
    formatter_ptr _formatter;
    level::level_enum _level = level::INFO;
};
}
}
