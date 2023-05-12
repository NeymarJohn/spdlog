#include "stdafx.h"
#include <memory.h>

#include "c11log/formatters/formatters.h"
#include "c11log/level.h"



static thread_local std::tm last_tm;
static thread_local char timestamp_cache[64];

void c11log::formatters::format_time(const c11log::formatters::timepoint& tp, std::ostream &dest)
{

    auto tm = details::os::localtime(std::chrono::system_clock::to_time_t(tp));
    // Cache timestamp string of last second
    if(memcmp(&tm, &last_tm, sizeof(tm)))
    {
		sprintf(timestamp_cache, "[%d-%02d-%02d %02d:%02d:%02d]", tm.tm_year + 1900,
			tm.tm_mon + 1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec);
		last_tm = tm;
    }
           
	dest << timestamp_cache;		
}

void c11log::formatters::format_time(std::ostream& dest)
{
    return format_time(std::chrono::system_clock::now(), dest);
}


static const char _hex_chars[17] = "0123456789ABCDEF";

std::string c11log::formatters::to_hex(const unsigned char* buf, std::size_t size)
{
    std::ostringstream oss;

    for (std::size_t i = 0; i < size; i++) {
        oss << _hex_chars[buf[i] >> 4];
        oss << _hex_chars[buf[i] & 0x0F];
    }
    return oss.str();
}
