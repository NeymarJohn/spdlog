
#include "spdlog/spdlog.h"


int main(int, char*[])
{
	namespace spd = spdlog;
	///Create a file rotating logger with 5mb size max and 3 rotated files
    auto logger = spd::rotating_logger_mt("file_logger", "logs/spd-sample", 10 *1024 * 1024 , 5);

	logger->set_pattern("[%Y-%b-%d %T.%e]: %v");
	for(int i  = 0 ; i < 1000000; ++i)
	    logger->info() << "spdlogger message #" << i;

    return 0;
}
