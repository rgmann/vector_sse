#include <string.h>
#include <emmintrin.h>
#ifdef __SSE4_1__  // modern CPU - use SSE 4.1
#include <smmintrin.h>
#endif
#include "vector_sse_vec_mul.h"

#define  SSE_VECTOR_WIDTH    (4)
// #define  EL_PER_VEC    SSE_VECTOR_WIDTH

static inline __m128i mul_s32(const __m128i* a, const __m128i* b )
{
#ifdef __SSE4_1__  // modern CPU - use SSE 4.1
    return _mm_mullo_epi32(*a, *b);
#else               // old CPU - use SSE 2
    __m128i tmp1 = _mm_mul_epu32(*a,*b); /* mul 2,0*/
    __m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(*a,4), _mm_srli_si128(*b,4)); /* mul 3,1 */
    return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0))); /* shuffle results to [63..0] and pack */
#endif
}

static inline __m128i mul_f32(const __m128i* a, const __m128i* b )
{
   return _mm_mul_ps( *a, *b );
}

static inline __m128i mul_f64(const __m128i* a, const __m128i* b )
{
   return _mm_mul_pd( *a, *b );
}

#define  TEMPLATE_VEC_MUL_S( FUNC_NAME, TYPE, TYPE_SIZE, CONV_IN, CONV_OUT, EL_PER_VEC, MULOP ) \
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
   __m128i left_vec;  \
   __m128i right_vec; \
\
   TYPE result_segment[ EL_PER_VEC ]; \
   __m128i result_vec; \
\
   __m128i sign_left;  \
   __m128i sign_right; \
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
         left_vec  = _mm_loadu_si128( (const __m128i *)left_segment );  \
         right_vec = _mm_loadu_si128( (const __m128i *)right_segment ); \
\
         result_vec = MULOP( &left_vec, &right_vec ); \
\
         _mm_store_si128( (__m128i*)result_segment, result_vec ); \
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


TEMPLATE_VEC_MUL_S( method_vec_mul_s32, int32_t, 32, NUM2INT, INT2NUM, 4, mul_s32 );
TEMPLATE_VEC_MUL_S( method_vec_mul_f32, float, 32, NUM2DBL, DBL2NUM, 4, mul_f32 );
TEMPLATE_VEC_MUL_S( method_vec_mul_f64, double, 64, NUM2DBL, DBL2NUM, 2, mul_f64 );

