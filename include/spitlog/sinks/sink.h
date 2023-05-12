#pragma once

#include "../details/log_msg.h"

namespace spitlog
{
namespace sinks
{
class sink
{
public:
    virtual ~sink() {}
    virtual void log(const details::log_msg& msg) = 0;
};
}
}

