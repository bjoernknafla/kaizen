#include <kaizen/kaizen_raw_frame_time.h>
#include <kaizen/kaizen_raw_reliable_frame_time_scope.h>
#include <kaizen/kaizen_stddef.h>

#include <cassert>

#include <UnitTest++.h>



namespace {
    
    int const monotonic_time_iteration_number = 1000000;
    
} // anonymous namespace


SUITE(kaizen_raw_frame_time_test)
{
    TEST(frame_time_supported)
    {
        CHECK_EQUAL(KAIZEN_TRUE, kaizen_frame_time_is_supported());
    }
    
    
    
    TEST(known_frame_time_resolution)
    {
        kaizen_frame_time_resolution_t resolution = kaizen_unknown_frame_time_resolution;
        int const errc = kaizen_frame_time_query_resolution(&resolution);
        assert(KAIZEN_SUCCESS == errc);
        
        CHECK(kaizen_unknown_frame_time_resolution != resolution);
    }
    
    
    
    TEST(usage)
    {
        kaizen_raw_reliable_frame_time_scope_t reliable_frame_time_scope;
        int error_code = kaizen_reliable_frame_time_scope_init(&reliable_frame_time_scope);
        assert(KAIZEN_SUCCESS == error_code);
        
        kaizen_raw_frame_time_t start = KAIZEN_RAW_FRAME_TIME_ZERO;
        kaizen_raw_frame_time_t stop = KAIZEN_RAW_FRAME_TIME_ZERO;
        
        error_code = kaizen_frame_time_query(&start);
        assert(error_code == KAIZEN_SUCCESS);
        
        error_code = kaizen_frame_time_query(&stop);
        assert(error_code == KAIZEN_SUCCESS);
        
        error_code = kaizen_reliable_frame_time_scope_finalize(&reliable_frame_time_scope);
        assert(KAIZEN_SUCCESS == error_code);
        
        
        kaizen_raw_frame_time_t difference = KAIZEN_RAW_FRAME_TIME_ZERO;
        error_code = kaizen_frame_time_difference(&start, &stop, &difference);
        assert(error_code == KAIZEN_SUCCESS);
        
        
        double delta = 0.0;
        error_code = kaizen_frame_time_convert_to_nanoseconds(&difference, 
                                                              &delta);
        assert(error_code == KAIZEN_SUCCESS);
        
        // Check that time is monotonic
        CHECK(delta >= 0.0);
    }
    
    
    TEST(monotonic_time)
    {
        kaizen_raw_reliable_frame_time_scope_t reliable_frame_time_scope;
        int error_code = kaizen_reliable_frame_time_scope_init(&reliable_frame_time_scope);
        assert(KAIZEN_SUCCESS == error_code);
        
        kaizen_raw_frame_time_t first = KAIZEN_RAW_FRAME_TIME_ZERO;
        kaizen_raw_frame_time_t second = KAIZEN_RAW_FRAME_TIME_ZERO;
        
        int errc = kaizen_frame_time_query(&first);
        assert(KAIZEN_SUCCESS == errc);
        
        for (int i = 0; i < monotonic_time_iteration_number; ++i) {
            int errcode = kaizen_frame_time_query(&second);
            assert(KAIZEN_SUCCESS == errcode);
            
            kaizen_raw_frame_time_t diff = KAIZEN_RAW_FRAME_TIME_ZERO;
            errcode = kaizen_frame_time_subtract(&second, &first, &diff);
            assert(KAIZEN_SUCCESS == errcode);
            
            double nanosecs = 0.0;
            errcode = kaizen_frame_time_convert_to_nanoseconds(&diff,
                                                               &nanosecs);
            assert(KAIZEN_SUCCESS == errcode);
            
            // Time differences mist always be greater or euqal to zero - aka
            // monotonic.
            CHECK(nanosecs >= 0.0);
            
            errcode = kaizen_frame_time_query(&first);
            assert(KAIZEN_SUCCESS == errcode);
        }
        
        error_code = kaizen_reliable_frame_time_scope_finalize(&reliable_frame_time_scope);
        assert(KAIZEN_SUCCESS == error_code);
    }
    
} // SUITE(kaizen_test)


