#include <string.h>
#include <emmintrin.h>
#include "vector_sse_sum.h"

#define  TEMPLATE_SUM_S( FUNC_NAME, TYPE, OFTYPE, TYPE_SIZE, CONV_IN, CONV_OUT, EL_PER_VEC, ADD ) \
VALUE FUNC_NAME( VALUE self, VALUE vector ) \
{ \
   uint32_t length      = 0; \
   uint32_t offset      = 0; \
   uint32_t vector_pos  = 0; \
   uint32_t input_index = 0; \
\
   TYPE  result = 0; \
\
   TYPE left_segment[ EL_PER_VEC ]; \
   TYPE right_segment[ EL_PER_VEC ]; \
   TYPE result_segment[ EL_PER_VEC ]; \
   TYPE vector_segment[ EL_PER_VEC ]; \
\
   __m128i left_vec; \
   __m128i right_vec; \
   __m128i result_vec; \
\
   __m128i sign_left; \
   __m128i sign_right; \
   const OFTYPE OVERFLOW_MASK = ( (OFTYPE)0x1 << (TYPE_SIZE-1) ); \
   OFTYPE overflow[ EL_PER_VEC ]; \
   __m128i* overflow_vec = (__m128i*)overflow; \
\
   Check_Type( vector, T_ARRAY ); \
\
   length = RARRAY_LEN( vector ); \
\
   if ( length > 0 ) \
   { \
      memset( &result_vec, 0, sizeof( result_vec ) ); \
\
      for ( offset = 0; offset < length; offset += EL_PER_VEC ) \
      { \
         for ( vector_pos = 0; vector_pos < EL_PER_VEC; ++vector_pos ) \
         { \
            input_index = offset + vector_pos; \
            if ( input_index < length ) \
            { \
               vector_segment[ vector_pos ] = CONV_IN( rb_ary_entry( vector, input_index ) ); \
            } \
            else \
            { \
               vector_segment[ vector_pos ] = 0; \
            } \
         } \
\
         right_vec = _mm_loadu_si128( (const __m128i *)vector_segment ); \
         left_vec  = _mm_loadu_si128( &result_vec ); \
\
         result_vec = ADD( left_vec, right_vec ); \
\
         sign_left = _mm_xor_si128(result_vec, left_vec); \
         sign_right = _mm_xor_si128(result_vec, right_vec); \
         *overflow_vec = _mm_and_si128(sign_left, sign_right); \
\
         for ( vector_pos = 0; vector_pos < EL_PER_VEC; ++vector_pos ) \
         { \
            if ( ( (OFTYPE)overflow[ vector_pos ] & OVERFLOW_MASK ) ) \
            { \
               rb_raise( rb_eRuntimeError, "Vector addition overflow" ); \
            } \
         } \
      } \
\
      _mm_store_si128( (__m128i*)result_segment, result_vec ); \
\
      for ( vector_pos = 0; vector_pos < EL_PER_VEC; ++vector_pos ) \
      { \
         result += result_segment[ vector_pos ]; \
      } \
   } \
\
   return CONV_OUT( result ); \
}

TEMPLATE_SUM_S( method_vec_sum_s32, int32_t, int32_t, 32, NUM2INT, INT2NUM, 4, _mm_add_epi32 );
TEMPLATE_SUM_S( method_vec_sum_s64, int64_t, int64_t, 64, NUM2LL, LL2NUM, 2, _mm_add_epi64 );
TEMPLATE_SUM_S( method_vec_sum_f32, float, int32_t, 32, NUM2DBL, DBL2NUM, 4, _mm_add_ps );
TEMPLATE_SUM_S( method_vec_sum_f64, double, int64_t, 32, NUM2DBL, DBL2NUM, 2, _mm_add_pd );

