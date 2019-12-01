#include "timestamp.h"

namespace mqt
{
namespace timer
{

Timestamp::Timestamp()
{
    memset(&timeval_, 0x00, sizeof timeval_); 
}

Timestamp::Timestamp(const struct timeval& tv)
    : timeval_(tv)
{

}

Timestamp::Timestamp(int64_t micro_seconds)
{
    timeval_.tv_sec = micro_seconds / 1000000;
	timeval_.tv_usec = micro_seconds % 1000000;
}

Timestamp::Timestamp(const Timestamp& timestamp)
    : timeval_(timestamp.timeval_)
{

}

Timestamp& Timestamp::operator=(const Timestamp& timestamp)
{
    if (this != &timestamp) 
    {
        timeval_ = timestamp.timeval_;
    }
    return *this;
}

void Timestamp::swap(Timestamp& timestamp)
{
    std::swap(timeval_.tv_sec, timestamp.timeval_.tv_sec);
	std::swap(timeval_.tv_usec, timestamp.timeval_.tv_usec);
}

std::string Timestamp::fmt_string(bool show_micro) const
{
    char buf[32] = {0};
	time_t seconds = static_cast<int64_t>(timeval_.tv_sec);
	struct tm t;
	localtime_r(&seconds, &t);
	//gmtime_r(&seconds, &t);
	if (!show_micro) 
	{
		snprintf(buf, sizeof buf, "%04d%02d%02d %02d:%02d:%02d", 
				t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
				t.tm_hour, t.tm_min, t.tm_sec);
	} 
	else 
	{
		int64_t microSeconds = static_cast<int64_t>(timeval_.tv_usec);
		snprintf(buf, sizeof buf, "%04d%02d%02d %02d:%02d:%02d.%06ld", 
				t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
				t.tm_hour, t.tm_min, t.tm_sec, 
				microSeconds);
	}
	return buf;
}

const struct timeval& Timestamp::timeval() const 
{ 
    return timeval_; 
}


time_t Timestamp::seconds() const 
{ 
    return timeval_.tv_sec; 
}

int64_t Timestamp::micro_seconds() const
{
    return static_cast<int64_t>(timeval_.tv_sec * 1000000L + timeval_.tv_usec);
}

bool Timestamp::valid() const
{
    return timeval_.tv_usec > 0;
}

Timestamp Timestamp::now()
{
    struct timeval tv;
	gettimeofday(&tv, nullptr);
	return Timestamp(tv);
}

Timestamp Timestamp::invalid()
{
	return Timestamp();
}

Timestamp Timestamp::from_unixtime(time_t t)
{
    struct timeval tv = {t, 0};
	return Timestamp(tv);
}


}

}