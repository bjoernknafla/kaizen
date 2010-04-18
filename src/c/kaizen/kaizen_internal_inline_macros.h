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
 * C inline support for different compilers and C versions if
 * functions can not be defined as static internal in the 
 * header file or if project wide support to enable or disable inlining
 * is needed.
 *
 * Usage: For your project define exactly one of the following macros:
 * KAIZEN_USE_C99_INLINE or KAIZEN_USE_GCC_INLINE or 
 * KAIZEN_USE_PRE_C99_AND_GCC_INLINE or KAIZEN_USE_PRE_C99_AND_MSVC_INLINE or
 * KAIZEN_USE_CPP_INLINE (for C++).
 *
 * Put all functions to inline into their own file which might have
 * the suffix .inl . Prefix all inline functions declarations and definitions
 * with the macro KAIZEN_INLINE.
 * This inline function file should not have typical header file header guard
 * macros instead add one section 
 * <code>
 * #if KAIZEN_ENABLE_IN_FILE_DECLARATION_SECTION == 1
 * // Put all function declarations for functions to inline here.
 * // Example: KAIZEN_INLINE int one_plus_one_func();
 * #endif
 * </code>
 * Beneath add another section:
 * <code>
 * #if KAIZEN_ENABLE_IN_FILE_DEFINITION_SECTION == 1
 * // Put all function definitions for functions to inline here.
 * // Example: KAIZEN_INLINE int one_plus_one_func() { return 2; }
 * #endif
 * </code>
 *  
 * In the header file into which you want to put the inline files first 
 * include kaizen_internal_inline_macros.h and then include the file with the
 * two sections for inline function declaration and definition.
 * To prevent the inline macro symbols from leaking into all files that 
 * include the header file include kaizen_internal_inline_macros_undef.h
 * afterwards.
 * 
 * In the source file that belongs to the header file just described first
 * define the macro KAIZEN_INLINE_INSIDE_SRC_FILE and then include
 * the file with the inline function declarations and definitions.
 * 
 * Voila, quite some afford but cross-compiler, cross-C-language-version
 * inline support for C.
 *
 * Macros defined here are undefined by including 
 * kaizen_internal_inline_macros_undef.h
 */

#ifndef KAIZEN_kaizen_internal_inline_macros_H
#define KAIZEN_kaizen_internal_inline_macros_H

#if defined(KAIZEN_INLINE_INSIDE_SRC_FILE) /* Inside source file */
#   if defined(KAIZEN_USE_C99_INLINE)
#       define KAIZEN_INLINE extern
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 1
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 0
#   elif defined(KAIZEN_USE_GCC_INLINE)
#       define KAIZEN_INLINE /* Nothing */
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 0
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 1
#   elif defined(KAIZEN_USE_PRE_C99_AND_GCC_INLINE)
#       define KAIZEN_INLINE /* Nothing */
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 0
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 1
#   elif defined(KAIZEN_USE_PRE_C99_AND_MSVC_INLINE) /* Internal linkage */
#       define KAIZEN_INLINE /* Nothing */
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 0
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 0
#   elif defined(KAIZEN_USE_CPP_INLINE) /* Internal linkage */
#       define KAIZEN_INLINE /* Nothing - must not happen happen */
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 0
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 0
#   else /* C89 or pre C89 */
#       define KAIZEN_INLINE /* Nothing - no inline used */
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 0
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 1
#   endif
#else /* Inside header file */
#   if defined(KAIZEN_USE_C99_INLINE)KAIZEN
#       define KAIZEN_INLINE inline
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 1
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 1
#   elif defined(KAIZEN_USE_GCC_INLINE)
#       define KAIZEN_INLINE inline extern
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 1
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 1
#   elif defined(KAIZEN_USE_PRE_C99_AND_GCC_INLINE)
#       define KAIZEN_INLINE __inline__ extern
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 1
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 1
#   elif defined(KAIZEN_USE_PRE_C99_AND_MSVC_INLINE) /* Internal linkage */
#       define KAIZEN_INLINE __inline
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 1
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 1
#   elif defined(KAIZEN_USE_CPP_INLINE)  /* Internal linkage */
#       define KAIZEN_INLINE inline
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 1
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 1
#   else /* C89 or pre C89 */
#       define KAIZEN_INLINE extern /* Nothing - no inline used */
#       define KAIZEN_ENABLE_INL_FILE_DECLARATION_SECTION 1
#       define KAIZEN_ENABLE_INL_FILE_DEFINITION_SECTION 0
#   endif
#endif


#endif /* KAIZEN_kaizen_internal_inline_macros_H */
