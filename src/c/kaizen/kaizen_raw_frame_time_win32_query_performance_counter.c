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
 * Implementation of kaizen_raw_time_frame functionality with
 * QueryPerformanceCounter and QueryPerformanceFrequency.
 *
 * See http://support.microsoft.com/kb/895980
 */

#include "kaizen_raw_frame_time.h"


#include <assert.h>
#include <errno.h>
#include <float.h>

#include <windows.h>


#include "kaizen_stddef.h"



inline static int kaizen_internal_frame_time_convert_with_factor(struct kaizen_raw_frame_time_s const* time, double const factor, double* result);
inline static int kaizen_internal_frame_time_convert_with_factor(struct kaizen_raw_frame_time_s const* time, double const factor, double* result)
{
    assert(NULL != time);
    assert(NULL != result);
    assert(time->counter.QuadPart >= 0);
    
    int return_code = ENOSYS;
    
    LARGE_INTEGER frequency = {(DWORD)0,(LONG)0};
    BOOL const errc = QueryPerformanceFrequency(&frequency);
    
    if (FALSE != errc) {
        
        LONGLONG const freq = frequency.QuadPart;
        LONGLONG const elapsed = time->counter.QuadPart;
        
        /* TODO: @todo Add a cast-overflow check in debug mode.
         *             Did not add it yet as LONGLONG definition seems to be
         *             platform dependent and hard to gather all infos for
         *             all platforms (Xbox360?).
         */
        
        double const secs = factor * (double)elapsed / (double)freq;
        
        *result = secs;
        
        return_code = KAIZEN_SUCCESS;
        
    } else {
        DWORD const last_error = GetLastError();
        assert(0);
        
        return_code = ENOSYS;
    }
    
    return return_code;
}



kaizen_bool kaizen_frame_time_is_supported(void)
{
    kaizen_bool return_value = KAIZEN_FALSE;
    
    LARGE_INTEGER dummy;
    BOOL const errc = QueryPerformanceFrequency(&dummy);
    
    if (FALSE != errc) {
        
        return_value = KAIZEN_TRUE;
    }
    
    return return_value;
}



kaizen_bool kaizen_frame_time_is_monotonic(void)
{
    return KAIZEN_TRUE;
}




int kaizen_frame_time_query_resolution(kaizen_frame_time_resolution_t* resolution)
{
    assert(NULL != resolution);
    
    int return_code = ENOSYS;
    
    LARGE_INTEGER frequency = {0,0};
    BOOL const errc = QueryPerformanceFrequency(&frequency);
    
    if (FALSE != errc) {
        
        LONGLONG const freq = frequency.QuadPart;
        
        if (1000 > freq) {
            *resolution = kaizen_seconds_frame_time_resolution;
        } else if (1000000 > freq) {
            *resolution = kaizen_milliseconds_frame_time_resolution;
        } else if (1000000000 > freq) {
            *resolution = kaizen_microseconds_frame_time_resolution;
        } else {
            *resolution = kaizen_nanoseconds_frame_time_resolution;
        }
        
        return_code = KAIZEN_SUCCESS;
    } else {
        DWORD const last_error = GetLastError();
        *resolution = kaizen_unknown_frame_time_resolution;
        return_code = ENOSYS;
    }
    
    return return_code;
}



int kaizen_frame_time_query(struct kaizen_raw_frame_time_s* now)
{
    assert(NULL != now);
    
    int return_code = ENOSYS;
    
    LARGE_INTEGER counter;
    BOOL const errc = QueryPerformanceCounter(&counter);
    
    /* Checking that errc is not FALSE because MSDN doc only states
     * that return value isn't zero if function call succeeded. 
     */
    if (FALSE != errc) {
        
        now->counter = counter;
        return_code = KAIZEN_SUCCESS;
        
    } else {
        
        DWORD const last_error = GetLastError();
        assert(TRUE);
        
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
    assert(later->counter.QuadPart >= 0);
    assert(earlier->counter.QuadPart >= 0);
    assert(later->counter.QuadPart >= earlier->counter.QuadPart);
    
    LONGLONG const later_counter = later->counter.QuadPart;
    LONGLONG const earlier_counter = earlier->counter.QuadPart;
    
    LONGLONG const difference = later_counter - earlier_counter;
    
    result->counter.QuadPart = difference;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_difference(struct kaizen_raw_frame_time_s const* one,
                                 struct kaizen_raw_frame_time_s const* two,
                                 struct kaizen_raw_frame_time_s* result)
{
    assert(NULL != one);
    assert(NULL != two);
    assert(NULL != result);
    assert(one->counter.QuadPart >= 0);
    assert(two->counter.QuadPart >= 0);
    
    LONGLONG const one_counter = one->counter.QuadPart;
    LONGLONG const two_counter = two->counter.QuadPart;
    
    LONGLONG elapsed = 0;
    
    if (one_counter >= two_counter) {
        elapsed = one_counter - two_counter;
    } else {
        elapsed = two_counter - one_counter;
    }
    
    result->counter.QuadPart = elapsed;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_aggregate(struct kaizen_raw_frame_time_s const* lhs,
                                struct kaizen_raw_frame_time_s const* rhs,
                                struct kaizen_raw_frame_time_s* result)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    assert(NULL != result);
    assert(lhs->counter.QuadPart >= 0);
    assert(rhs->counter.QuadPart >= 0);
    
    LONGLONG const left_counter = lhs->counter.QuadPart;
    LONGLONG const right_counter = rhs->counter.QuadPart;
    
    LONGLONG const aggregate = left_counter + right_counter;
    
    assert(aggregate >= left_counter
           && aggregate >= right_counter
           && "Overflow");
    
    result->counter.QuadPart = aggregate;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_convert_to_nanoseconds(struct kaizen_raw_frame_time_s const* time,
                                             double* result)
{
    return kaizen_internal_frame_time_convert_with_factor(time, 
                                                          1.0e+9, 
                                                          result);
}



int kaizen_frame_time_convert_to_microseconds(struct kaizen_raw_frame_time_s const* time,
                                              double* result)
{
    return kaizen_internal_frame_time_convert_with_factor(time, 
                                                          1.0e+6, 
                                                          result);
}



int kaizen_frame_time_convert_to_milliseconds(struct kaizen_raw_frame_time_s const* time,
                                              double* result)
{
    return kaizen_internal_frame_time_convert_with_factor(time, 
                                                          1.0e+3, 
                                                          result);
}



int kaizen_frame_time_convert_to_seconds(struct kaizen_raw_frame_time_s const* time,
                                         double* result)
{
    return kaizen_internal_frame_time_convert_with_factor(time, 
                                                          1.0, 
                                                          result);
}



kaizen_bool kaizen_frame_time_equal(struct kaizen_raw_frame_time_s const* lhs,
                                    struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->counter.QuadPart == rhs->counter.QuadPart;
}



kaizen_bool kaizen_frame_time_unequal(struct kaizen_raw_frame_time_s const* lhs,
                                      struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->counter.QuadPart != rhs->counter.QuadPart;
}




kaizen_bool kaizen_frame_time_greater(struct kaizen_raw_frame_time_s const* lhs,
                                      struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->counter.QuadPart > rhs->counter.QuadPart;
}




kaizen_bool kaizen_frame_time_greater_or_equal(struct kaizen_raw_frame_time_s const* lhs,
                                               struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->counter.QuadPart >= rhs->counter.QuadPart;
}




kaizen_bool kaizen_frame_time_lesser(struct kaizen_raw_frame_time_s const* lhs,
                                     struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->counter.QuadPart < rhs->counter.QuadPart;
}




kaizen_bool kaizen_frame_time_lesser_or_equal(struct kaizen_raw_frame_time_s const* lhs,
                                              struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->counter.QuadPart <= rhs->counter.QuadPart;
}


