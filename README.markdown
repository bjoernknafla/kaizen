kaizen - runtime profiling for games
------------------------------------

*kaizen* is a cross platform *C* toolkit to implement runtime profiling for 
computer and video games. `kaizen_frame_time_`functions encapsulate 
high precision timer or counter of the target platform. Times to measure are
typically very small, below a second, to measure runtimes of functions and
computations during a cycle of the game main loop which normally run with
30 - 60 Hz.

*kaizen* focuses on support for profiling of parallelized code bases.

All functions and types use the `kaizen_` prefix, while preprocessor symbols use  
`KAIZEN_`.  

No platform specific headers and symbols are included by `kaizen/kaizen.h`. 
However you can access headers containing `_raw_` in their file name. These are  
shallow wrappers around the platforms thread functionality and include platform  
headers.  

The code can be downloaded at: [http://github.com/bjoernknafla/kaizen/](http://github.com/bjoernknafla/kaizen/)  
Please file issues with the code at: [http://github.com/bjoernknafla/kaizen/issues/](http://github.com/bjoernknafla/kaizen/issues)  


### Warning ###

*kaizen* just started to aggregate in code - and it will change, transform, and
grow before it is really usable.


### Author(s) and Contact ###

You have got questions, need to critisize me, or just want to say hello? I am 
looking forward to hear from you!

Bjoern Knafla  
Bjoern Knafla Parallelization + AI + Gamedev Consulting  
[kaizen@bjoernknafla.com](mailto:kaizen@bjoernknafla.com)  
[www.bjoernknafla.com](http://www.bjoernknafla.com)  


### Copyright and License ###

*kaizen* is free software. You may copy, distribute, and modify it under the terms
of the license contained in the file COPYRIGHT.txt distributed with this
package. This license is equal to the Simplified BSD License.


### Building ###

To build *kaizen* you have got the following options:

 *  Define `KAIZEN_USE_APPLE_MACH_ABSOLUTE_TIME` and only compile the generic C source files
    and the C files ending in `_apple_mac_absolute_time.c` to build for Apple platforms.    
    
 *  Define `KAIZEN_USE_POSIX_CLOCK_GETTIME` and only compile generic C files and C source
    files ending in `_posix_clock_gettime.c` to build for POSIX compliant platforms 
    supporting `clock_gettime`, `clock_getres`, and `CLOCK_REALTIME`.
 
 *  Define `KAIZEN_USE_WIN32_QUERY_PERFORMANCE_COUNTER` and only
    compile generic C source files, C files ending in `_win32_query_performance_counter.c` to
    build for Windows OS.


### Disclaimer ###

All trademarks copyrights belong to their respective trademark holders and
copyright owners.


### Release Notes ###

#### Version 0.0.1 (April 18, 2010) ####

 *  Initial release

