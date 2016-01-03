#ifndef  VECTOR_SSE_COMMON_H
#define  VECTOR_SSE_COMMON_H

#include <emmintrin.h>

__m128i add_f32( const __m128i left, const __m128i right );
__m128i add_f64( const __m128i left, const __m128i right );
__m128i sub_f32( const __m128i left, const __m128i right );
__m128i sub_f64( const __m128i left, const __m128i right );
__m128i mul_f32( const __m128i left, const __m128i right );
__m128i mul_f64( const __m128i left, const __m128i right );

#endif // VECTOR_SSE_COMMON_H