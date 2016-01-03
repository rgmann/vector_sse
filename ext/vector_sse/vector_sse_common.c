#include "vector_sse_common.h"

__m128i add_f32( const __m128i left, const __m128i right )
{
   return _mm_castps_si128( _mm_add_ps( _mm_castsi128_ps( left ), _mm_castsi128_ps( right ) ) );
}

__m128i add_f64( const __m128i left, const __m128i right )
{
   return _mm_cvtpd_epi32( _mm_add_pd( _mm_castsi128_pd( left ), _mm_castsi128_pd( right ) ) );
}

__m128i sub_f32( const __m128i left, const __m128i right )
{
   return _mm_castps_si128( _mm_sub_ps( _mm_castsi128_ps( left ), _mm_castsi128_ps( right ) ) );
}

__m128i sub_f64( const __m128i left, const __m128i right )
{
   return _mm_cvtpd_epi32( _mm_sub_pd( _mm_castsi128_pd( left ), _mm_castsi128_pd( right ) ) );
}

__m128i mul_f32( const __m128i left, const __m128i right )
{
   return _mm_castps_si128( _mm_mul_ps( _mm_castsi128_ps( left ), _mm_castsi128_ps( right ) ) );
}

__m128i mul_f64( const __m128i left, const __m128i right )
{
   return _mm_cvtpd_epi32( _mm_mul_pd( _mm_castsi128_pd( left ), _mm_castsi128_pd( right ) ) );
}
