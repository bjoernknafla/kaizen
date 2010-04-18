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
 * Implementation of kaizen_raw_time_frame functionality with Apple's
 * mach_absolute_time.
 *
 * See http://developer.apple.com/mac/library/qa/qa2004/qa1398.html
 * See http://developer.apple.com/iphone/library/qa/qa2009/qa1643.html
 * See http://www.macresearch.org/tutorial_performance_and_time
 * See http://www.wand.net.nz/~smr26/wordpress/2009/01/19/monotonic-time-in-mac-os-x/
 * See http://developer.apple.com/mac/library/documentation/Darwin/Conceptual/KernelProgramming/services/services.html#//apple_ref/doc/uid/TP30000905-CH219-CHDGFEFE
 *
 * TODO: @todo Only query mach_timebase_info once per app execution.
 */

#include "kaizen_raw_frame_time.h"


#include <assert.h>
#include <errno.h>
#include <float.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>


#include "kaizen_stddef.h"



static int kaizen_internal_frame_time_convert_to_nanoseconds_uint64(struct kaizen_raw_frame_time_s const* time,
                                                           uint64_t* result);
int kaizen_internal_frame_time_convert_to_nanoseconds_uint64(struct kaizen_raw_frame_time_s const* time,
                                                    uint64_t* result)
{
    assert(NULL != time);
    assert(NULL != result);
    
    int return_code = EAGAIN;
    
    mach_timebase_info_data_t timebase;
    kern_return_t const errc = mach_timebase_info(&timebase);
    
    if (KERN_SUCCESS == errc) {
        
        uint64_t const elapsed = time->interval;
        *result = elapsed * (uint64_t)timebase.numer / (uint64_t)timebase.denom;
        return_code = KAIZEN_SUCCESS;
    } else {
        /**
         * Error code is not correct and only used to signal that an error
         * occured at all.
         *
         * Xnu sources indicate that only KERN_SUCCESS is returned by
         * mach_timebase_info.
         *
         * TODO: @todo Find out what kind of errors might be reported by 
         *             mach_timebase_info to return correct error code.
         */
        
        return_code = EAGAIN;
    }
    
    
    return return_code;
}



/* Internal helper function to convert frame time via a factor into  
 * non-computer-internal time units.
 */
inline static int kaizen_internal_frame_time_convert_with_factor(struct kaizen_raw_frame_time_s const* time, double const conversion_factor, double* result);
inline static int kaizen_internal_frame_time_convert_with_factor(struct kaizen_raw_frame_time_s const* time, double const conversion_factor, double* result)
{
    assert(NULL != time);
    assert(NULL != result);
    
    uint64_t intermediate_result = 0;
    
    int errc = kaizen_internal_frame_time_convert_to_nanoseconds_uint64(time, 
                                                               &intermediate_result);
    
    if (KAIZEN_SUCCESS == errc) {
        assert(0 == (intermediate_result & ((uint64_t)(-1))<< DBL_MANT_DIG));
        *result = conversion_factor * (double)intermediate_result;
    }
    
    return errc;
}



kaizen_bool kaizen_frame_time_is_supported(void)
{
    kaizen_bool return_value = KAIZEN_FALSE;
    
    mach_timebase_info_data_t timebase;
    kern_return_t const errc = mach_timebase_info(&timebase);
    
    if (KERN_SUCCESS == errc) {
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
    
    *resolution = kaizen_nanoseconds_frame_time_resolution;
 
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_query(struct kaizen_raw_frame_time_s* now)
{
    assert(NULL != now);
    
    now->interval = mach_absolute_time();
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_subtract(struct kaizen_raw_frame_time_s const* later,
                               struct kaizen_raw_frame_time_s const* earlier,
                               struct kaizen_raw_frame_time_s* result)
{
    assert(NULL != later);
    assert(NULL != earlier);
    assert(NULL != result);
    assert(later->interval >= earlier->interval);
    
    uint64_t const later_interval = later->interval;
    uint64_t const earlier_interval = earlier->interval;
    
    uint64_t const difference = later_interval - earlier_interval;
    
    result->interval = difference;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_difference(struct kaizen_raw_frame_time_s const* one,
                                 struct kaizen_raw_frame_time_s const* two,
                                 struct kaizen_raw_frame_time_s* result)
{
    assert(NULL != one);
    assert(NULL != two);
    assert(NULL != result);
    
    uint64_t const one_interval = one->interval;
    uint64_t const two_interval = two->interval;
    
    uint64_t elapsed = 0;
    
    if (one_interval >= two_interval) {
        elapsed = one_interval - two_interval;
    } else {
        elapsed = two_interval - one_interval;
    }
    
    result->interval = elapsed;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_aggregate(struct kaizen_raw_frame_time_s const* lhs,
                                struct kaizen_raw_frame_time_s const* rhs,
                                struct kaizen_raw_frame_time_s* result)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    assert(NULL != result);
    
    uint64_t const left_interval = lhs->interval;
    uint64_t const right_interval = rhs->interval;
    
    uint64_t const aggregate = left_interval + right_interval;
    
    assert(aggregate >= left_interval
           && aggregate >= right_interval
           && "Overflow");
    
    result->interval = aggregate;
    
    return KAIZEN_SUCCESS;
}



int kaizen_frame_time_convert_to_nanoseconds(struct kaizen_raw_frame_time_s const* time,
                                             double* result)
{
    return kaizen_internal_frame_time_convert_with_factor(time, 1.0, result);
}



int kaizen_frame_time_convert_to_microseconds(struct kaizen_raw_frame_time_s const* time,
                                              double* result)
{
    return kaizen_internal_frame_time_convert_with_factor(time, 1.0e-3, result);
}



int kaizen_frame_time_convert_to_milliseconds(struct kaizen_raw_frame_time_s const* time,
                                              double* result)
{
    return kaizen_internal_frame_time_convert_with_factor(time, 1.0e-6, result);
}



int kaizen_frame_time_convert_to_seconds(struct kaizen_raw_frame_time_s const* time,
                                         double* result)
{
    return kaizen_internal_frame_time_convert_with_factor(time, 1.0e-9, result);
}



kaizen_bool kaizen_frame_time_equal(struct kaizen_raw_frame_time_s const* lhs,
                                    struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->interval == rhs->interval;
}



kaizen_bool kaizen_frame_time_unequal(struct kaizen_raw_frame_time_s const* lhs,
                                      struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->interval != rhs->interval;
}




kaizen_bool kaizen_frame_time_greater(struct kaizen_raw_frame_time_s const* lhs,
                                      struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->interval > rhs->interval;
}




kaizen_bool kaizen_frame_time_greater_or_equal(struct kaizen_raw_frame_time_s const* lhs,
                                               struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->interval >= rhs->interval;
}




kaizen_bool kaizen_frame_time_lesser(struct kaizen_raw_frame_time_s const* lhs,
                                     struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->interval < rhs->interval;
}




kaizen_bool kaizen_frame_time_lesser_or_equal(struct kaizen_raw_frame_time_s const* lhs,
                                              struct kaizen_raw_frame_time_s const* rhs)
{
    assert(NULL != lhs);
    assert(NULL != rhs);
    
    return lhs->interval <= rhs->interval;
}


