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
 * Measuring time with kaizen_frame_time can be unreliable on certain
 * platforms (Win32) if the thread which measures time gets 
 * scheduled to another processor core so start and stop times are
 * measured on different cores.
 *
 * To minimize the chance for wrong time measurements initialize 
 * a kaizen_raw_reliable_frame_time_scope before measuring time and
 * finalize if after all measurments relative to the first one are done.
 *
 * A reliability scope can be embedded into an outer scope but inner scopes
 * must be initialized and finished inside the outer scopes. It is best to
 * only have one reliability scope open as long as needed.
 *
 * Opening and closing a kaizen_raw_reliable_frame_time_scope is costly on
 * some platforms and changes the way threads are scheduled on some platforms
 * therefore don't initialize and finalize scopes too often but also do not
 * keep them open too long as the thread scheduling might be affected.
 *
 * If the threads are controlled by a thread or task pool and the thread/task
 * pool already set the thread affinity it isn't necessary to use
 * kaizen_reliable_frame_time_scope_init and _finalize.
 * 
 * See http://msdn.microsoft.com/en-us/library/ms644904(VS.85).aspx
 * See http://msdn.microsoft.com/en-us/library/ms644905(v=VS.85).aspx
 * See http://msdn.microsoft.com/en-us/library/ms686247(VS.85).aspx
 *
 * Thanks to Daniel Stephens (@auscoder on Twitter) and Rick Molloy
 * (@rickmolloy on Twitter) for their advice to use SetThreadAffinity
 * on Win32 when using QueryPerformanceCounter.
 */

#ifndef KAIZEN_kaizen_raw_reliable_frame_time_scope_H
#define KAIZEN_kaizen_raw_reliable_frame_time_scope_H


#if defined(KAIZEN_USE_WIN32_QUERY_PERFORMANCE_COUNTER)
#   include <windows.h>
#else
#   /* Nothing to do */
#endif



#if defined(__cplusplus)
extern "C" {
#endif

    
    struct kaizen_raw_reliable_frame_time_scope_s {
#if defined(KAIZEN_USE_WIN32_QUERY_PERFORMANCE_COUNTER)
        DWORD_PTR last_thread_affinity_mask;
#else
        void* dummy;
#endif
    };
    typedef struct kaizen_raw_reliable_frame_time_scope_s kaizen_raw_reliable_frame_time_scope_t;
    
    
    /**
     * On certain platforms (Win32) pins the thread on its
     * preferred processor core until
     * kaizen_reliable_frame_time_scope_finalize is called.
     *
     * See remarks in the header documentation, too.
     */
    int kaizen_reliable_frame_time_scope_init(struct kaizen_raw_reliable_frame_time_scope_s* scope);

    /**
     * On certain platforms (Win32) it unpins the calling thread
     * from the processor core selected by 
     * kaizen_reliable_frame_time_scope_init and sets the 
     * setting before the init call again.
     *
     * See remarks in the header documentation, too.
     */
    int kaizen_reliable_frame_time_scope_finalize(struct kaizen_raw_reliable_frame_time_scope_s* scope);

    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
 

#endif /* KAIZEN_kaizen_raw_reliable_frame_time_scope_H */
