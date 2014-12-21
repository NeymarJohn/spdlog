# spdlog

Very fast, header only, C++ logging library.

## Install
Just copy the files to your build tree and use a C++11 compiler

## Tested on:
* gcc 4.8.1 and above
* clang 3.5
* visual studio 2013
* mingw with g++ 4.9.x

##Features
* Very fast - performance is the primary goal (see [becnhmarks](#benchmarks) below).
* Headers only.
* No dependencies - just copy and use.
* Cross platform - Linux / Windows on 32/64 bits.
* **new!** Feature rich [call style](#usage-example) using the excellent [cppformat](http://cppformat.github.io/) library.
* ostream call style is supported too.
* Extremely fast asynchronous mode (optional) - use of lockfree queues and other tricks to reach millions  of calls per second from multiple threads.
* [Custom](https://github.com/gabime/spdlog/wiki/Custom-formatting) formatting.
* Multi/Single threaded loggers.
* Various log targets:
    * Rotating log files.
    * Daily log files.
    * Console logging.
    * Linux syslog.
    * Easily extendable with custom log targets  (just implement a single function in the [sink](include/spdlog/sinks/sink.h) interface).
* Severity based filtering - threshold levels can be modified in runtime.



## Benchmarks

Below are some [benchmarks](bench) comparing popular log libraries under Ubuntu 64 bit, Intel i7-4770 CPU @ 3.40GHz 

#### Synchronous mode
Time needed to log 1,000,000 lines under synchronous mode (in seconds, the best of 3 runs):

|threads|boost log|glog   |easylogging |spdlog|
|-------|:-------:|:-----:|----------:|------:|
|1|       4.169s  |1.066s |0.975s     |0.302s|
|10|     16.029   |3.032s |2.857s     |0.968s|
|100|     15.008  |1.139s |4.512s     |0.497s|


#### Asynchronous mode 
Time needed to log 1,000,000 lines under asynchronous mode, i.e. the time it takes to put them in the async queue (in seconds, the best of 3 runs):

|threads|g2log   |spdlog<sup>async mode</sup>|
|:-------|:-----:|-------------------------:|
|1|       1.850s |0.216s |
|10|      0.943s  |0.173s|
|100|      0.959s |0.202s|




## Usage Example
```c++
#include <iostream>
#include "spdlog/spdlog.h"

int main(int, char* [])
{
    namespace spd = spdlog;
    try
    {
        // Set log level to all loggers to DEBUG and above
        spd::set_level(spd::level::debug);

        //Create console, multithreaded logger
        auto console = spd::stdout_logger_mt("console");
        console->info("Welcome to spdlog!") ;
        console->info("An info message example {}..", 1);
        console->info() << "Streams are supported too  " << 1;

        console->info("Easy padding in numbers like {:08d}", 12);
        console->info("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
        console->info("Support for floats {:03.2f}", 1.23456);
        console->info("Positional args are {1} {0}..", "too", "supported");

        console->info("{:<30}", "left aligned");
        console->info("{:>30}", "right aligned");
        console->info("{:^30}", "centered");
       
        // Create a file rotating logger with 5mb size max and 3 rotated files
        auto file_logger = spd::rotating_logger_mt("file_logger", "logs/mylogfile", 1048576 * 5, 3);
        file_logger->set_level(spd::level::info);
        for(int i = 0; i < 10; ++i)
		      file_logger->info("{} * {} equals {:>10}", i, i, i*i);

        // Customize msg format for all messages
        spd::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");
        file_logger->info("This is another message with custom format");

        spd::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name) function");

        SPDLOG_TRACE(console, "Enabled only #ifdef SPDLOG_TRACE_ON..{} ,{}", 1, 3.23);
        SPDLOG_DEBUG(console, "Enabled only #ifdef SPDLOG_DEBUG_ON.. {} ,{}", 1, 3.23);

        //
        // Asynchronous logging is very fast..
        // Just call spdlog::set_async_mode(q_size) and all created loggers from now on will be asynchronous..
        //
        size_t q_size = 1048576; //queue size must be power of 2
        spdlog::set_async_mode(q_size);
        auto async_file= spd::daily_logger_st("async_file_logger", "logs/async_log.txt");
        async_file->info() << "This is async log.." << "Should be very fast!";
               
        // syslog example. linux only..
        #ifdef __linux__
        std::string ident = "spdlog-example";
        auto syslog_logger = spd::syslog_logger("syslog", ident, LOG_PID);
        syslog_logger->warn("This is warning that will end up in syslog. This is Linux only!");       
        #endif

    }
    catch (const spd::spdlog_ex& ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
}
```
