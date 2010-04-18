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
 * Implementation for kaizen_raw_reliable_frame_time_scope.h for Win32.
 *
 * See http://msdn.microsoft.com/en-us/library/ms686247(v=VS.85).aspx
 * See http://msdn.microsoft.com/en-us/library/ms686253(v=VS.85).aspx
 */

#include "kaizen_raw_reliable_frame_time_scope.h"

#include <windows.h>

#include <stddef.h>

#include "kaizen_stddef.h"



int kaizen_reliable_frame_time_scope_init(struct kaizen_raw_reliable_frame_time_scope_s* scope)
{
    assert(NULL != scope);
    
    if (NULL == scope) {
        return EINVAL;
    }
    
    int return_code = EAGAIN;
    
    HANDLE current_thread = GetCurrentThread();
    
    /* TODO: @todo Check if the ideal processor is valid by 
     *             looking at the process affinity mask.
     */
    DWORD ideal_processor = SetThreadIdealProcessor(current_thread,
                                                    MAXIMUM_PROCESSORS);
    
    if ((DWORD)-1 == ideal_processor) {
        DWORD const last_error = GetLastError();
        assert((DWORD)-1 != ideal_processor);
    }
    
    DWORD_PTR ideal_processor_affinity_mask = (DWORD_PTR)((uintptr_t)1 << ideal_processor);

    DWORD_PTR last_mask = SetThreadAffinityMask(current_thread,
                                                ideal_processor_affinity_mask); 
    
    if (NULL != last_mask) {
        scope->last_thread_affinity_mask = last_mask;
        return_code = KAIZEN_SUCCESS;
    } else {
        DWORD const last_error = GetLastError();
        assert(ERROR_INVALID_PARAMETER != last_error);
        
        return_code = EAGAIN;
    }
    
    return return_code;
}



int kaizen_reliable_frame_time_scope_finalize(struct kaizen_raw_reliable_frame_time_scope_s* scope)
{
    /* Nothing to do */
    assert(NULL != scope);

    if (NULL == scope) {
        return EINVAL;
    }
    
    int return_code = EAGAIN;
    
    DWORD_PTR last_mask = SetThreadAffinityMask(GetCurrentThread(), 
                                                scope->last_thread_affinity_mask);
    
    if (NULL != last_mask) {
        return_code = KAIZEN_SUCCESS;
    } else {
        DWORD const last_error = GetLastError();
        assert(ERROR_INVALID_PARAMETER != last_error);
        
        return_code = ESRCH;
    }
    
    return return_code;
}



