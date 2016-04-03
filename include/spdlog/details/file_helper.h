//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once

// Helper class for file sink
// When failing to open a file, retry several times(5) with small delay between the tries(10 ms)
// Can be set to auto flush on every line
// Throw spdlog_ex exception on errors

#include <spdlog/details/os.h>
#include <spdlog/details/log_msg.h>

#include <chrono>
#include <cstdio>
#include <string>
#include <thread>

namespace spdlog
{
namespace details
{

class file_helper
{
public:
    const int open_tries = 5;
    const int open_interval = 10;

    explicit file_helper(bool force_flush) :
        _fd(nullptr),
        _force_flush(force_flush)
    {}

    file_helper(const file_helper&) = delete;
    file_helper& operator=(const file_helper&) = delete;

    ~file_helper()
    {
        close();
    }


    void open(const filename_str_t& fname, bool truncate = false)
    {

        close();
        const filename_char_t* mode = truncate ? SPDLOG_FILENAME_T("wb") : SPDLOG_FILENAME_T("ab");
        _filename = fname;
        for (int tries = 0; tries < open_tries; ++tries)
        {
            if (!os::fopen_s(&_fd, fname, mode))
                return;

            std::this_thread::sleep_for(std::chrono::milliseconds(open_interval));
        }

        throw spdlog_ex("Failed opening file " + filename_to_bytes(_filename) + " for writing");
    }

    void reopen(bool truncate)
    {
        if (_filename.empty())
            throw spdlog_ex("Failed re opening file - was not opened before");
        open(_filename, truncate);

    }

    void flush()
    {
        std::fflush(_fd);
    }

    void close()
    {
        if (_fd)
        {
            std::fclose(_fd);
            _fd = nullptr;
        }
    }

    void write(const log_msg& msg)
    {

        size_t msg_size = msg.formatted.size();
        auto data = msg.formatted.data();
        if (std::fwrite(data, 1, msg_size, _fd) != msg_size)
            throw spdlog_ex("Failed writing to file " + filename_to_bytes(_filename));

        if (_force_flush)
            std::fflush(_fd);

    }

    long size()
    {
        if (!_fd)
            throw spdlog_ex("Cannot use size() on closed file " + filename_to_bytes(_filename));

        auto pos = ftell(_fd);
        if (fseek(_fd, 0, SEEK_END) != 0)
            throw spdlog_ex("fseek failed on file " + filename_to_bytes(_filename));

        auto file_size = ftell(_fd);

        if(fseek(_fd, pos, SEEK_SET) !=0)
            throw spdlog_ex("fseek failed on file " + filename_to_bytes(_filename));

        if (file_size == -1)
            throw spdlog_ex("ftell failed on file " + filename_to_bytes(_filename));


        return file_size;


    }

    const filename_str_t& filename() const
    {
        return _filename;
    }

    static bool file_exists(const filename_str_t& name)
    {

        return os::file_exists(name);
    }



private:
    FILE* _fd;
    filename_str_t _filename;
    bool _force_flush;


};
}
}
