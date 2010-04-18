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
 * Cross-platform high precision (independent from wall clock time)
 * time measurement targeted at measuring times during one main loop cycle
 * (frame) of an interactive computer or video game. Based on the platform the
 * functions strive to use a monotonic clock/counter if available.
 *
 * On some platforms it can happen that the time measured leaps because of 
 * hardware bugs.
 *
 * If a platform doesn't provide a monotonic clock or counter changes of the
 * system time can influence (increase or decrease) the time measured. The 
 * kaizen library is targeted at runtime profiling of games and short time
 * spans, typicall inside a frame of a game loop, therefore system time changes
 * shouldn't occur very often and will only affect a single frame. Watch out
 * for strange timing results.
 *
 * Monotonic timers (if availabel on the platform) aren't affected by process 
 * context switches or the time a process or thread sleeps or is blocked and 
 * therefore include these non-execution times in the measurement. System time
 * changes do not affect monotonic timers.
 * 
 * On some platforms (e.g. WindowsXP and the Xbox360) it is adviced to only
 * measure times from one (the main) thread.
 *
 * On non-problematic hardware measuring time from different threads should
 * work. It's best to only relate (subtract or aggregate) times from the 
 * same thread and to measure times inside a reliable frame time scope (see
 * kaizen_raw_reliable_frame_time_scope.h).
 *
 * Keep an eye on the timing values measured to detect errors and keep them  
 * in perspective.
 *
 * See http://msdn.microsoft.com/en-us/library/ee417693%28VS.85%29.aspx
 */

#ifndef KAIZEN_kaizen_raw_frame_time_H
#define KAIZEN_kaizen_raw_frame_time_H


#include <kaizen/kaizen_stddef.h>



#if defined(KAIZEN_USE_APPLE_MACH_ABSOLUTE_TIME)
#   include <stdint.h>
#elif defined(KAIZEN_USE_POSIX_GETTIMEOFDAY)
#   error Unsupported platform.
#elif defined(KAIZEN_USE_POSIX_CLOCK_GETTIME)
#   include <time.h>
#elif defined(KAIZEN_USE_WIN32_QUERY_PERFORMANCE_COUNTER)
#   include <windows.h>
#else
#   error Unsupported platform.
#endif


#if defined(__cplusplus)
extern "C" {
#endif


    enum kaizen_frame_time_resolution {
        kaizen_unknown_frame_time_resolution = 0,
        kaizen_nanoseconds_frame_time_resolution,
        kaizen_microseconds_frame_time_resolution,
        kaizen_milliseconds_frame_time_resolution,
        kaizen_seconds_frame_time_resolution
    };
    typedef enum kaizen_frame_time_resolution kaizen_frame_time_resolution_t;

    
    
    /**
     * Time interval to measure game times inside frames.
     * 
     * Treat as opaque type and do not rely on implementation details.
     *
     * @attention Only usable for short time spans below seconds.
     */
    struct kaizen_raw_frame_time_s {
#if defined(KAIZEN_USE_APPLE_MACH_ABSOLUTE_TIME)
        uint64_t interval;
#elif defined(KAIZEN_USE_POSIX_GETTIMEOFDAY)
#   error Unsupported platform.
#elif defined(KAIZEN_USE_POSIX_CLOCK_GETTIME)
        timespec time; 
#elif defined(KAIZEN_USE_WIN32_QUERY_PERFORMANCE_COUNTER)
        LARGE_INTEGER counter;
#else
#   error Unsupported platform.
#endif
    };
    typedef struct kaizen_raw_frame_time_s kaizen_raw_frame_time_t;
    
    
#if defined(KAIZEN_USE_APPLE_MACH_ABSOLUTE_TIME)
#   define KAIZEN_RAW_FRAME_TIME_ZERO {0}
#elif defined(KAIZEN_USE_POSIX_GETTIMEOFDAY)
#   error Unsupported platform.
#elif defined(KAIZEN_USE_POSIX_CLOCK_GETTIME)
#   define KAIZEN_RAW_FRAME_TIME_ZERO {(time_t)0, (long)0}
#elif defined(KAIZEN_USE_WIN32_QUERY_PERFORMANCE_COUNTER)
#   define KAIZEN_RAW_FRAME_TIME_ZERO {(DWORD)0, (LONG)0}
#else
#   error Unsupported platform.
#endif
    
    
    
    /**
     * Returns KAIZEN_TRUE if the platform has a high resolution timer or
     * counter and therefore frame time measurements are supported, otherwise
     * returns KAIZEN_FALSE.
     */
    kaizen_bool kaizen_frame_time_is_supported(void);
    
    
    /**
     * Returns KAIZEN_TRUE if the frame time measured is based on a monotonic
     * timer, otherwise returns KAIZEN_FALSE.
     */
    kaizen_bool kaizen_frame_time_is_monotonic(void);
    
    /**
     * Queries the platform for the resolution of the high precision
     * frame time used.
     *
     * Regardless of the platforms capabilities, nanoseconds are the highest
     * resolution reported and seconds the lowest.
     *
     * On some platforms this is computational not cheap as a kernel
     * trap might be necessary to query the timer/counter.
     *
     * Parameter must not be NULL.
     */
    int kaizen_frame_time_query_resolution(kaizen_frame_time_resolution_t* resolution);
    
    
    
    int kaizen_frame_time_query(struct kaizen_raw_frame_time_s* now);
    
    
    /**
     * Calculates the time difference between @a later and @a earlier.
     *
     * @attention later must have been queried after earlier.
     * @attention On certain platforms simple use of kaizen_frame_time
     *            functionality can lead to wrong time measurement on parallel
     *            processors. Use kaizen_frame_timer or kaizen_frame_stop_watch
     *            for correct time measurement.
     *
     * All parameters must not be NULL.
     */
    int kaizen_frame_time_subtract(struct kaizen_raw_frame_time_s const* later,
                                   struct kaizen_raw_frame_time_s const* earlier,
                                   struct kaizen_raw_frame_time_s* result);
    
    /**
     * Calculates the absolute difference bettern one and two regardless of
     * which was measured before or after the other.
     *
     * @attention On certain platforms simple use of kaizen_frame_time
     *            functionality can lead to wrong time measurement on parallel
     *            processors. Use kaizen_frame_timer or kaizen_frame_stop_watch
     *            for correct time measurement.
     *
     * All parameters must not be NULL.
     */
    int kaizen_frame_time_difference(struct kaizen_raw_frame_time_s const* one,
                                     struct kaizen_raw_frame_time_s const* two,
                                     struct kaizen_raw_frame_time_s* result);
    
    /**
     * Aggregates the time measured in lhs and rhs and stores it into result.
     *
     * All parameters must not be NULL.
     */
    int kaizen_frame_time_aggregate(struct kaizen_raw_frame_time_s const* lhs,
                                    struct kaizen_raw_frame_time_s const* rhs,
                                    struct kaizen_raw_frame_time_s* result);
    
    
    /**
     *
     * Based on platform this can be an expensive operation. To convert many 
     * time values use the functionality based on kaizen_frame_time_converter.
     *
     * All parameters must not be NULL.
     */
    int kaizen_frame_time_convert_to_nanoseconds(struct kaizen_raw_frame_time_s const* time,
                                                 double* result);
    
    int kaizen_frame_time_convert_to_microseconds(struct kaizen_raw_frame_time_s const* time,
                                                  double* result);
    
    int kaizen_frame_time_convert_to_milliseconds(struct kaizen_raw_frame_time_s const* time,
                                                  double* result);
    
    int kaizen_frame_time_convert_to_seconds(struct kaizen_raw_frame_time_s const* time,
                                             double* result);
    
    
    /* TODO: @todo Implement
     * Document that times must be positive or zero.
     * Document that because of floating point precision some values might not
     * result in expected internal time values.
     * TODO: @todo Decide if to only offer conversion from nanoseconds.
     * TODO: @todo Decide if to offer double and uint types to convert from.
    int kaizen_frame_time_convert_from_nanoseconds(struct kaizen_raw_frame_time_s* result,
                                                   double nanoseconds);
    
    int kaizen_frame_time_convert_from_microseconds(struct kaizen_raw_frame_time_s* result,
                                                    double microseconds);
    
    int kaizen_frame_time_convert_from_milliseconds(struct kaizen_raw_frame_time_s* result,
                                                    double milliseconds);
    
    int kaizen_frame_time_convert_from_seconds(struct kaizen_raw_frame_time_s* result,
                                               double seconds);
    */
    
    kaizen_bool kaizen_frame_time_equal(struct kaizen_raw_frame_time_s const* lhs,
                                        struct kaizen_raw_frame_time_s const* rhs);
    
    kaizen_bool kaizen_frame_time_unequal(struct kaizen_raw_frame_time_s const* lhs,
                                          struct kaizen_raw_frame_time_s const* rhs);
    
    kaizen_bool kaizen_frame_time_greater(struct kaizen_raw_frame_time_s const* lhs,
                                          struct kaizen_raw_frame_time_s const* rhs);
    
    kaizen_bool kaizen_frame_time_greater_or_equal(struct kaizen_raw_frame_time_s const* lhs,
                                                   struct kaizen_raw_frame_time_s const* rhs);
    
    kaizen_bool kaizen_frame_time_lesser(struct kaizen_raw_frame_time_s const* lhs,
                                         struct kaizen_raw_frame_time_s const* rhs);
    
    kaizen_bool kaizen_frame_time_lesser_or_equal(struct kaizen_raw_frame_time_s const* lhs,
                                                  struct kaizen_raw_frame_time_s const* rhs);
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* KAIZEN_kaizen_raw_frame_time_H */
