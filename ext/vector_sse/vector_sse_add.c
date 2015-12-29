#include <emmintrin.h>
#include "vector_sse_add.h"

#define  TEMPLATE_ADD_S( FUNC_NAME, TYPE, OFTYPE, TYPE_SIZE, CONV_IN, CONV_OUT, EL_PER_VEC, ADD ) \
VALUE FUNC_NAME( VALUE self, VALUE left, VALUE right ) \
{ \
   uint32_t length = 0; \
   uint32_t offset = 0; \
   uint32_t vector_pos = 0; \
   uint32_t input_index = 0; \
\
   TYPE left_segment[ EL_PER_VEC ]; \
   TYPE right_segment[ EL_PER_VEC ]; \
   TYPE result_segment[ EL_PER_VEC ]; \
\
   __m128i left_vec; \
   __m128i right_vec; \
   __m128i* result_vec = (__m128i*)result_segment; \
\
   __m128i sign_left;  \
   __m128i sign_right; \
   const OFTYPE OVERFLOW_MASK = ( (OFTYPE)0x1 << (TYPE_SIZE-1) ); \
   TYPE overflow[ EL_PER_VEC ]; \
   __m128i* overflow_vec = (__m128i*)overflow; \
\
   VALUE result = Qnil; \
\
   Check_Type( left, T_ARRAY ); \
   Check_Type( right, T_ARRAY ); \
\
   if ( RARRAY_LEN( left ) != RARRAY_LEN( right ) ) \
   { \
      rb_raise( rb_eRuntimeError, "Vector lengths must be the same" ); \
   } \
\
   length = RARRAY_LEN( left ); \
   result = rb_ary_new2( length ); \
\
   if ( length > 0 ) \
   { \
      for ( offset = 0; offset < length; offset += EL_PER_VEC ) \
      { \
         for ( vector_pos = 0; vector_pos < EL_PER_VEC; ++vector_pos ) \
         { \
            input_index = offset + vector_pos; \
            if ( input_index < length ) \
            { \
               left_segment[ vector_pos ] = CONV_IN( rb_ary_entry( left, input_index ) ); \
               right_segment[ vector_pos ] = CONV_IN( rb_ary_entry( right, input_index ) ); \
            } \
            else \
            { \
               left_segment[ vector_pos ] = 0; \
               right_segment[ vector_pos ] = 0; \
            } \
         } \
\
         left_vec = _mm_loadu_si128( (const __m128i *)left_segment ); \
         right_vec = _mm_loadu_si128( (const __m128i *)right_segment ); \
         *result_vec = ADD( left_vec, right_vec ); \
\
         sign_left = _mm_xor_si128(*result_vec, left_vec); \
         sign_right = _mm_xor_si128(*result_vec, right_vec); \
         *overflow_vec = _mm_and_si128(sign_left, sign_right); \
\
         for ( vector_pos = 0; vector_pos < EL_PER_VEC; ++vector_pos ) \
         { \
            if ( 0 && ( (OFTYPE)overflow[ vector_pos ] & OVERFLOW_MASK ) ) \
            { \
               rb_raise( rb_eRuntimeError, "Vector addition overflow" ); \
            } \
\
            input_index = offset + vector_pos; \
\
            if ( input_index < length ) \
            { \
               rb_ary_push( result, CONV_OUT( result_segment[ vector_pos ] ) ); \
            } \
         } \
      } \
   } \
\
   return result; \
}


TEMPLATE_ADD_S( method_vec_add_s32, int32_t, int32_t, 32, NUM2INT, INT2NUM, 4, _mm_add_epi32 );
TEMPLATE_ADD_S( method_vec_add_s64, int64_t, int64_t, 64, NUM2LL, LL2NUM, 2, _mm_add_epi64 );
TEMPLATE_ADD_S( method_vec_add_f32, float, int32_t, 32, NUM2DBL, DBL2NUM, 4, _mm_add_ps );
TEMPLATE_ADD_S( method_vec_add_f64, double, int64_t, 64, NUM2DBL, DBL2NUM, 2, _mm_add_pd );

TEMPLATE_ADD_S( method_vec_sub_s32, int32_t, int32_t, 32, NUM2INT, INT2NUM, 4, _mm_sub_epi32 );
TEMPLATE_ADD_S( method_vec_sub_s64, int64_t, int64_t, 64, NUM2LL, LL2NUM, 2, _mm_sub_epi64 );
TEMPLATE_ADD_S( method_vec_sub_f32, float, int32_t, 32, NUM2DBL, DBL2NUM, 4, _mm_sub_ps );
TEMPLATE_ADD_S( method_vec_sub_f64, double, int64_t, 64, NUM2DBL, DBL2NUM, 2, _mm_sub_pd );
