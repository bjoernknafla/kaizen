/*
 * Copyright (c) 2010, Bjoern Knafla
 * http://www.bjoernknafla.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 *
 *   * Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Bjoern Knafla 
 *     Parallelization + AI + Gamedev Consulting nor the names of its 
 *     contributors may be used to endorse or promote products derived from 
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * 
 * Implementation of kaizen frame time measuring functionality with
 * POSIX clock_gettime.
 *
 * See http://www.opengroup.org/onlinepubs/007908799/xsh/clock_gettime.html
 * See http://www.wand.net.nz/~smr26/wordpress/2009/01/19/monotonic-time-in-mac-os-x/
 * See http://www.opengroup.org/onlinepubs/007908799/xsh/realtime.html
 *
 * To use clock_gettime and clock_getres link against librt.
 */

#include "kaizen_raw_frame_time.h"


#include <assert.h>
#include <errno.h>
#include <float.h>

#include <time.h>

#error Not tested yet.

#if !defined(CLOCK_REALTIME)
#   error POSIX realtime clock not supported on platform.
#endif


#define KAIZEN_INTERNAL_ONE_SECOND_IN_NANOSECONDS 1000000000


inline static kaizen_bool kaizen_internal_timespec_is_valid(struct timespec const* time);
inline static kaizen_bool kaizen_internal_timespec_is_valid(struct timespec const* time)
{
    return ((time->tv_sec >= 0)
            && (time->tv_nsec < KAIZEN_INTERNAL_ONE_SECOND_IN_NANOSECONDS)
            && (time->tv_nsec > 0));
}



kaizen_bool kaizen_frame_time_is_supported(void)
{
    kaizen_bool return_value = KAIZEN_TRUE;
    
    struct timespec time;
    int const error_indicator = clock_getres(CLOCK_REALTIME, &time);
    assert(kaizen_internal_timespec_is_valid(&time));
    
    if (0 != error_indicator) {
        int const errc = errno;
        return_value = KAIZEN_FALSE;
    }
    
    return return_value;
}



kaizen_bool kaizen_frame_time_is_monotonic(void)
{
    return KAIZEN_FALSE;
}




int kaizen_frame_time_query_resolution(kaizen_frame_time_resolution_t* resolution)
{
    assert(NULL != resolution);
    
    int return_code = ENOSYS;
    
    struct timespec res;
    int const error_indicator = clock_getres(CLOCK_REALTIME, &res);
    assert(KAIZEN_TRUE == kaizen_internal_timespec_is_valid(&res));
    
    /* TODO: @todo Unsure how to handle the opaque tv_sec value. Decide to
     *             use gmtime or localtime or keep the current implementation.
     */
    long const sec_res = (long)res.tv_sec;
    long const nanosec_res = res.tv_nsec;
    
    if (0 == error_indicator) {
        
        kaizen_frame_time_resolution_t result = kaizen_unknown_frame_time_resolution;
        
#error Untested functionality.
        /* TODO: @todo Check logic on linux computer. */
        if (sec_res > 0 /* || nanosec_res >= 1000000000 */) {
            result = kaizen_seconds_frame_time_resolution;
        } else if (nanosec_res >= 1000000) {
            result = kaizen_milliseconds_frame_time_resolution;
        } else if (nanosec_res >= 1000) {
            result = kaizen_microseconds_frame_time_resolution;
        } else if (nanosec_res > 0) {
            result = kaizen_nanoseconds_frame_time_resolution;
        } else {
            result = kaizen_unknown_frame_time_resolution;
        }
        
        *resolution = result;
        
        return_code = KAIZEN_SUCCESS;
    } else {
        int const errc = errno;
        assert(0);
        return_code = ENOSYS;
    }
    
    
    return return_code;
}



int kaizen_frame_time_query(struct kaizen_raw_frame_time_s* now)
{
    assert(NULL != now);
    
    int return_code = ENOSYS;
    
    struct timespec time = KAIZEN_RAW_FRAME_TIME_ZERO;
    int const error_indicator = clock_gettime(CLOCK_REALTIME, &time);
    assert(kaizen_internal_timespec_is_valid(&time));
    
    if (0 == error_indicator) {
        
        now->time = time;
        
        return_code = KAIZEN_SUCCESS;
    } else {
        int const errc = errno;
        assert(0);
        return_code = ENOSYS;
    }
    
    return return_code;
}



int kaizen_frame_time_subtract(struct kaizen_raw_frame_time_s const* later,
                               struct kaizen_raw_frame_time_s const* earlier,
                               struct kaizen_raw_frame_time_s* result)
{
    assert(NULL != later);
    assert(NULL != earlier);
    assert(NULL != result);
    assert(KAIZEN_TRUE == kaizen_frame_time_greater_or_equal(later, earlier));
    assert(kaizen_internal_timespec_is_valid(&(later->time)));
    assert(kaizen_internal_timespec_is_valid(&(earlier->time)));
    
    struct timespec const later_time = later->time;
    struct timespec const earlier_time = earlier->time;
    
    time_t const difference_tv_sec = later_time.tv_sec - earlier_time.tv_sec; 
    long const difference_tv_nsec = later_time.tv_nsec - earlier_time.tv_nsec;
    struct timespec const difference = {difference_tv_sec, difference_tv_nsec};
    
    result->time = difference;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_difference(struct kaizen_raw_frame_time_s const* one,
                                 struct kaizen_raw_frame_time_s const* two,
                                 struct kaizen_raw_frame_time_s* result)
{
    assert(NULL != one);
    assert(NULL != two);
    assert(NULL != result);
    assert(kaizen_internal_timespec_is_valid(&(one->time)));
    assert(kaizen_internal_timespec_is_valid(&(two->time)));
    
    int return_code = ENOSYS;
    
    if (KAIZEN_TRUE == kaizen_frame_time_greater_or_equal(one, two)) {
        
        return_code = kaizen_frame_time_subtract(one, two, result);
    } else {
        return_code = kaizen_frame_time_subtract(two, one, result);
    }
    
    
    return return_code;
}



int kaizen_frame_time_aggregate(struct kaizen_raw_frame_time_s const* lhs,
                                struct kaizen_raw_frame_time_s const* rhs,
                                struct kaizen_raw_frame_time_s* result)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    assert(NULL != result);
    assert(kaizen_internal_timespec_is_valid(&(lhs->time)));
    assert(kaizen_internal_timespec_is_valid(&(rhs->time)));
    
    struct timespec const left_time = lhs->time;
    struct timespec const right_time = rhs->time;
    
    long sec_aggregate = left_time.tv_sec + right_time.tv_sec;
    long nssec_aggregate = left_time.tv_nsec + right_time.tv_nsec;
    
    if (nsec_aggregate >= KAIZEN_INTERNAL_ONE_SECOND_IN_NANOSECONDS) {
        sec_aggregate += 1;
        nsec_aggregate -= KAIZEN_INTERNAL_ONE_SECOND_IN_NANOSECONDS;
    }
    
    struct timespec const aggregate = {sec_aggregate, nsec_aggregate};
    
    assert(kaizen_frame_time_greater_or_equal(&aggregate, &left_time)
           && kaizen_frame_time_greater_or_equal(&aggregate, &right_time)
           && "Overflow");
    
    result->time = aggregate;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_convert_to_nanoseconds(struct kaizen_raw_frame_time_s const* time,
                                             double* result)
{
    assert(NULL != time);
    assert(NULL != result);
    assert(kaizen_internal_timespec_is_valid(&(time->time)));
    
    *result = 1000000000.0 * (double)time->tv_sec + (double)time->tv_nsec;

    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_convert_to_microseconds(struct kaizen_raw_frame_time_s const* time,
                                              double* result)
{
    assert(NULL != time);
    assert(NULL != result);
    assert(kaizen_internal_timespec_is_valid(&(time->time)));
    
    *result = 1000000.0 * (double)time->tv_sec + (double)time->tv_nsec / 1000.0;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_convert_to_milliseconds(struct kaizen_raw_frame_time_s const* time,
                                              double* result)
{
    assert(NULL != time);
    assert(NULL != result);
    assert(kaizen_internal_timespec_is_valid(&(time->time)));
    
    *result = 1000.0 * (double)time->tv_sec + (double)time->tv_nsec / 1000000.0;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_convert_to_seconds(struct kaizen_raw_frame_time_s const* time,
                                         double* result)
{
    assert(NULL != time);
    assert(NULL != result);
    assert(kaizen_internal_timespec_is_valid(&(time->time)));
    
    *result = (double)time->tv_sec + (double)time->tv_nsec / 1000000000.0;
    
    return KAIZEN_SUCCESS;
}



kaizen_bool kaizen_frame_time_equal(struct kaizen_raw_frame_time_s const* lhs,
                                    struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    assert(kaizen_internal_timespec_is_valid(&(lhs->time)));
    assert(kaizen_internal_timespec_is_valid(&(rhs->time)));
    
    return (rhs->time.tv_sec == lhs->time.tv_sec
            && rhs->time.tv_nsec == lhs->time.tv_nsec);
}



kaizen_bool kaizen_frame_time_unequal(struct kaizen_raw_frame_time_s const* lhs,
                                      struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    assert(kaizen_internal_timespec_is_valid(&(lhs->time)));
    assert(kaizen_internal_timespec_is_valid(&(rhs->time)));
    
    return (rhs->time.tv_sec != lhs->time.tv_sec
            && rhs->time.tv_nsec != lhs->time.tv_nsec);
}




kaizen_bool kaizen_frame_time_greater(struct kaizen_raw_frame_time_s const* lhs,
                                      struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    assert(kaizen_internal_timespec_is_valid(&(lhs->time)));
    assert(kaizen_internal_timespec_is_valid(&(rhs->time)));
    
    return (rhs->time.tv_sec > lhs->time.tv_sec
            || ((rhs->time.tv_sec == lhs->time.tv_sec)
                && (rhs->time.tv_nsec > lhs->time.tv_nsec)));
}




kaizen_bool kaizen_frame_time_greater_or_equal(struct kaizen_raw_frame_time_s const* lhs,
                                               struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    assert(kaizen_internal_timespec_is_valid(&(lhs->time)));
    assert(kaizen_internal_timespec_is_valid(&(rhs->time)));
    
    return (rhs->time.tv_sec >= lhs->time.tv_sec
            || ((rhs->time.tv_sec == lhs->time.tv_sec)
                && (rhs->time.tv_nsec >= lhs->time.tv_nsec)));
}




kaizen_bool kaizen_frame_time_lesser(struct kaizen_raw_frame_time_s const* lhs,
                                     struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    assert(kaizen_internal_timespec_is_valid(&(lhs->time)));
    assert(kaizen_internal_timespec_is_valid(&(rhs->time)));
    
    return (rhs->time.tv_sec < lhs->time.tv_sec
            || ((rhs->time.tv_sec == lhs->time.tv_sec)
                && (rhs->time.tv_nsec < lhs->time.tv_nsec)));
}




kaizen_bool kaizen_frame_time_lesser_or_equal(struct kaizen_raw_frame_time_s const* lhs,
                                              struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    assert(kaizen_internal_timespec_is_valid(&(lhs->time)));
    assert(kaizen_internal_timespec_is_valid(&(rhs->time)));
    
    return (rhs->time.tv_sec <= lhs->time.tv_sec
            || ((rhs->time.tv_sec == lhs->time.tv_sec)
                && (rhs->time.tv_nsec <= lhs->time.tv_nsec)));
}


