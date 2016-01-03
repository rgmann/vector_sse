//
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 

#include <string.h>
#include <emmintrin.h>
#ifdef __SSE4_1__  // modern CPU - use SSE 4.1
#include <smmintrin.h>
#endif
#include "vector_sse_vec_mul.h"
#include "vector_sse_common.h"

#define  SSE_VECTOR_WIDTH    (4)
// #define  EL_PER_VEC    SSE_VECTOR_WIDTH

static inline __m128i mul_s32( const __m128i* a, const __m128i* b )
{
#ifdef __SSE4_1__
    return _mm_mullo_epi32(*a, *b);
#else               // old CPU - use SSE 2
    __m128i tmp1 = _mm_mul_epu32(*a,*b); /* mul 2,0*/
    __m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(*a,4), _mm_srli_si128(*b,4)); /* mul 3,1 */
    return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0))); /* shuffle results to [63..0] and pack */
#endif
}

static inline __m128i mul_s64( const __m128i* left_vec, const __m128i* right_vec  )
{
   // a * b = ( a0 * S + a1 ) * ( b0 * S + b1 )
   //       = a0 * b0 * S^2 + ( a0 * b1 + a1 * b0 ) * S + a1 * b1

   int64_t left[ 2 ];
   int64_t right[ 2 ];
   int64_t result[ 2 ];
   _mm_store_si128( (__m128i*)left, *left_vec );
   _mm_store_si128( (__m128i*)right, *right_vec );
   result[0] = left[0] * right[1];
   result[1] = left[1] * right[1];
   return _mm_loadu_si128( (const __m128i *)result );
}

static inline __m128 mul_f32_ptr(const __m128* left, const __m128* right )
{
   return _mm_mul_ps( *left, *right );
}

static inline __m128d mul_f64_ptr(const __m128d* left, const __m128d* right )
{
   return _mm_mul_pd( *left, *right );
}


#define  TEMPLATE_VEC_MUL_S( \
   FUNC_NAME, TYPE, CONV_IN, CONV_OUT, EL_PER_VEC, ITYPE, LOAD, LCAST, STORE, SCAST, MULOP ) \
VALUE FUNC_NAME( VALUE self, VALUE left, VALUE right ) \
{ \
   uint32_t length      = 0; \
   uint32_t offset      = 0; \
   uint32_t vector_pos  = 0; \
   uint32_t input_index = 0; \
\
   VALUE  result = Qnil; \
\
   TYPE left_segment[ EL_PER_VEC ];  \
   TYPE right_segment[ EL_PER_VEC ]; \
\
   ITYPE left_vec;  \
   ITYPE right_vec; \
\
   TYPE result_segment[ EL_PER_VEC ]; \
   ITYPE result_vec; \
\
   Check_Type( left, T_ARRAY );  \
   Check_Type( right, T_ARRAY ); \
\
   length = RARRAY_LEN( left );    \
   result = rb_ary_new2( length ); \
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
\
            if ( input_index < length ) \
            { \
               left_segment[ vector_pos ] = CONV_IN( rb_ary_entry( left, input_index ) );   \
               right_segment[ vector_pos ] = CONV_IN( rb_ary_entry( right, input_index ) ); \
            } \
            else \
            { \
               left_segment[ vector_pos ]  = 0; \
               right_segment[ vector_pos ] = 0; \
            } \
         } \
\
         left_vec  = LOAD( (const LCAST *)left_segment );  \
         right_vec = LOAD( (const LCAST *)right_segment ); \
\
         result_vec = MULOP( &left_vec, &right_vec ); \
\
         STORE( ( SCAST * )result_segment, result_vec ); \
\
         for ( vector_pos = 0; vector_pos < EL_PER_VEC; ++vector_pos ) \
         { \
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


TEMPLATE_VEC_MUL_S(
   method_vec_mul_s32,
   int32_t,
   NUM2INT, INT2NUM,
   4,
   __m128i,
   _mm_loadu_si128, __m128i,
   _mm_store_si128, __m128i,
   mul_s32 );
TEMPLATE_VEC_MUL_S(
   method_vec_mul_s64,
   int64_t,
   NUM2LL, LL2NUM,
   2,
   __m128i,
   _mm_loadu_si128, __m128i,
   _mm_store_si128, __m128i,
   mul_s64 );
TEMPLATE_VEC_MUL_S(
   method_vec_mul_f32,
   float,
   NUM2DBL, DBL2NUM,
   4,
   __m128,
   _mm_load_ps, float,
   _mm_store_ps, float,
   mul_f32_ptr );
TEMPLATE_VEC_MUL_S(
   method_vec_mul_f64,
   double,
   NUM2DBL, DBL2NUM,
   2,
   __m128d,
   _mm_load_pd, double,
   _mm_store_pd, double,
   mul_f64_ptr );

