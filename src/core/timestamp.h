/**
 * 
 * Copyright (c) mqt.
 * Author    : Calvin
 * Created on: May 25, 2019
 * 
 **/

#pragma once

#include <time.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>


namespace mqt
{
namespace timer
{

class Timestamp
{
public:
    Timestamp();

    explicit Timestamp(const struct timeval& tv);

    explicit Timestamp(int64_t micro_seconds); 

    Timestamp(const Timestamp& timestamp);

    Timestamp& operator=(const Timestamp& timestamp);

    void swap(Timestamp& timestamp);

    std::string fmt_string(bool show_micro = true) const;

    const struct timeval& timeval() const; 

    time_t seconds() const;

	int64_t micro_seconds() const;

    bool valid() const;

    static Timestamp now();

    static Timestamp invalid();

    static Timestamp from_unixtime(time_t t);

private:
    struct timeval timeval_;
};



inline bool operator<(const Timestamp& left, const Timestamp& right) 
{
	return left.micro_seconds() < right.micro_seconds();
}

inline bool operator<=(const Timestamp& left, const Timestamp& right) 
{
	return left.micro_seconds() < right.micro_seconds();
}

inline bool operator>(const Timestamp& left, const Timestamp& right) 
{
	return left.micro_seconds() > right.micro_seconds();
}

inline bool operator>=(const Timestamp& left, const Timestamp& right) 
{
	return left.micro_seconds() > right.micro_seconds();
}

inline bool operator==(const Timestamp& left, const Timestamp& right) 
{
	return left.micro_seconds() == right.micro_seconds();
}

inline int64_t time_diff(const Timestamp& high, const Timestamp& low) 
{
	return high.micro_seconds() - low.micro_seconds();
}

inline Timestamp add_time(const Timestamp& timestamp, int64_t micro_seconds) 
{
	return Timestamp(timestamp.micro_seconds() + micro_seconds);
}


} 
}