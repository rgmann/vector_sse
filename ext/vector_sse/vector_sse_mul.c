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

#include <emmintrin.h>
#include "vector_sse_mul.h"

#define  SSE_VECTOR_WIDTH    (4)

VALUE method_mat_mul_s32( VALUE self, VALUE left, VALUE left_rows_rb, VALUE left_cols_rb, VALUE right, VALUE right_rows_rb, VALUE right_cols_rb )
{
   uint32_t left_row = 0;
   uint32_t right_col = 0;
   uint32_t common = 0;
   uint32_t vector_pos = 0;
   uint32_t input_index = 0;
   uint32_t pos = 0;

   int32_t left_segment[ SSE_VECTOR_WIDTH ];
   int32_t right_segment[ SSE_VECTOR_WIDTH ];
   int64_t result_segment[ SSE_VECTOR_WIDTH/2 ];

   __m128i* left_vec = NULL;
   __m128i* right_vec = NULL;
   __m128i result_vec;

   VALUE result = Qnil;

   int64_t* result_native = NULL;

   uint32_t left_rows = NUM2INT( left_rows_rb );
   uint32_t left_cols = NUM2INT( left_cols_rb );
   uint32_t right_rows = NUM2INT( right_rows_rb );
   uint32_t right_cols = NUM2INT( right_cols_rb );

   uint32_t left_length = left_rows * left_cols;
   uint32_t right_length = right_rows * right_cols;
   uint32_t result_length = left_rows * right_cols;

   int32_t* left_native = NULL;
   int32_t* right_native = NULL;
   int64_t* partial_native = NULL;
   int64_t* temp = NULL;

   left_native  = (int32_t*) malloc( left_length * sizeof(int32_t) );
   right_native = (int32_t*) malloc( right_length * sizeof(int32_t) );
   result_native = (int64_t*) malloc( result_length * sizeof(int64_t) );
   partial_native = (int64_t*) malloc( left_cols * sizeof(int64_t) );

   memset(partial_native,0,left_cols*sizeof(int64_t) );

   for ( pos = 0; pos < left_length; ++pos )
   {
      left_native[ pos ] = NUM2INT( rb_ary_entry( left, pos ) );
   }
   for ( pos = 0; pos < right_length; ++pos )
   {
      right_native[ pos ] = NUM2INT( rb_ary_entry( right, pos ) );
   }

   for ( left_row = 0; left_row < left_rows; ++left_row )
   {
      for ( right_col = 0; right_col < right_cols; ++right_col )
      {
         for ( common = 0; common < left_cols; common += (SSE_VECTOR_WIDTH/2) )
         {
            memset( left_segment, 0, sizeof( left_segment ) );
            memset( right_segment, 0, sizeof( right_segment ) );

            input_index = common;
            left_segment[ 0 ] = left_native[ left_row * left_cols + input_index ];
            right_segment[ 0 ] = right_native[ input_index * right_cols + right_col ];

            input_index = common + 1;
            if ( input_index < left_cols )
            {
               left_segment[ 2 ] = left_native[ left_row * left_cols + input_index ];
               right_segment[ 2 ] = right_native[ input_index * right_cols + right_col ];
            }

            left_vec  = ( __m128i *)left_segment;
            right_vec = ( __m128i *)right_segment;
            result_vec = _mm_mul_epu32( *left_vec, *right_vec );

            _mm_store_si128( (__m128i*)result_segment, result_vec );
            for ( pos = 0; pos < SSE_VECTOR_WIDTH/2; ++pos )
            {
               if ( (common + pos) < left_cols )
               {
                  partial_native[ common + pos ] = result_segment[ pos ];
               }
            }
         }

         result_native[ left_row * right_cols + right_col ] = 0;
         temp = &result_native[ left_row * right_cols + right_col ];
         for ( common = 0; common < left_cols; ++common )
         {
            (*temp) += partial_native[ common ];
         }
      }
   }

   result = rb_ary_new2( result_length );
   for ( pos = 0; pos < result_length; ++pos )
   {
      rb_ary_push( result, INT2NUM( result_native[ pos ] ) );
   }

   free( left_native );
   free( right_native );
   free( result_native );
   free( partial_native );

   return result;
}

VALUE method_mat_mul_s64( VALUE self, VALUE left, VALUE left_rows_rb, VALUE left_cols_rb, VALUE right, VALUE right_rows_rb, VALUE right_cols_rb )
{
   uint32_t left_row = 0;
   uint32_t right_col = 0;
   uint32_t common = 0;
   uint32_t vector_pos = 0;
   uint32_t input_index = 0;
   uint32_t pos = 0;

   int64_t left_segment[ SSE_VECTOR_WIDTH ];
   int64_t right_segment[ SSE_VECTOR_WIDTH ];

   __m128i* left_vec = NULL;
   __m128i* right_vec = NULL;
   __m128i result_vec;

   VALUE result = Qnil;

   int64_t* result_native = NULL;

   uint32_t left_rows = NUM2INT( left_rows_rb );
   uint32_t left_cols = NUM2INT( left_cols_rb );
   uint32_t right_rows = NUM2INT( right_rows_rb );
   uint32_t right_cols = NUM2INT( right_cols_rb );

   uint32_t left_length = left_rows * left_cols;
   uint32_t right_length = right_rows * right_cols;
   uint32_t result_length = left_rows * right_cols;

   int64_t* left_native = NULL;
   int64_t* right_native = NULL;
   int64_t* partial_native = NULL;
   int64_t* temp = NULL;


   left_native  = (int64_t*) malloc( left_length * sizeof(int64_t) );
   right_native = (int64_t*) malloc( right_length * sizeof(int64_t) );
   result_native = (int64_t*) malloc( result_length * sizeof(int64_t) );
   partial_native = (int64_t*) malloc( left_cols * sizeof(int64_t) );

   memset(partial_native,0,left_cols*sizeof(int64_t) );

   for ( pos = 0; pos < left_length; ++pos )
   {
      left_native[ pos ] = NUM2LL( rb_ary_entry( left, pos ) );
   }
   for ( pos = 0; pos < right_length; ++pos )
   {
      right_native[ pos ] = NUM2LL( rb_ary_entry( right, pos ) );
   }

   for ( left_row = 0; left_row < left_rows; ++left_row )
   {
      for ( right_col = 0; right_col < right_cols; ++right_col )
      {
         for ( common = 0; common < left_cols; ++common )
         {
            partial_native[ common ] =
               left_native[ left_row * left_cols + common ] *
               right_native[ common * right_cols + right_col ];
         }

         result_native[ left_row * right_cols + right_col ] = 0;
         temp = &result_native[ left_row * right_cols + right_col ];
         for ( common = 0; common < left_cols; ++common )
         {
            (*temp) += partial_native[ common ];
         }
      }
   }

   result = rb_ary_new2( result_length );
   for ( pos = 0; pos < result_length; ++pos )
   {
      rb_ary_push( result, LL2NUM( result_native[ pos ] ) );
   }

   free( left_native );
   free( right_native );
   free( result_native );
   free( partial_native );

   return result;
}

VALUE method_mat_mul_f32( VALUE self, VALUE left, VALUE left_rows_rb, VALUE left_cols_rb, VALUE right, VALUE right_rows_rb, VALUE right_cols_rb )
{
   uint32_t left_row = 0;
   uint32_t right_col = 0;
   uint32_t common = 0;
   uint32_t vector_pos = 0;
   uint32_t input_index = 0;
   uint32_t pos = 0;

   float left_segment[ SSE_VECTOR_WIDTH ];
   float right_segment[ SSE_VECTOR_WIDTH ];
   float result_segment[ SSE_VECTOR_WIDTH ];

   __m128i* left_vec = NULL;
   __m128i* right_vec = NULL;
   __m128i result_vec;

   VALUE result = Qnil;

   float* result_native = NULL;

   uint32_t left_rows = NUM2UINT( left_rows_rb );
   uint32_t left_cols = NUM2UINT( left_cols_rb );
   uint32_t right_rows = NUM2UINT( right_rows_rb );
   uint32_t right_cols = NUM2UINT( right_cols_rb );

   uint32_t left_length = left_rows * left_cols;
   uint32_t right_length = right_rows * right_cols;
   uint32_t result_length = left_rows * right_cols;

   float* left_native = NULL;
   float* right_native = NULL;
   float* partial_native = NULL;
   float* temp = NULL;

   left_native  = (float*) malloc( left_length * sizeof(float) );
   right_native = (float*) malloc( right_length * sizeof(float) );
   result_native = (float*) malloc( result_length * sizeof(float) );
   partial_native = (float*) malloc( left_cols * sizeof(float) );

   memset( partial_native, 0, left_cols * sizeof(float) );

   for ( pos = 0; pos < left_length; ++pos )
   {
      left_native[ pos ] = NUM2DBL( rb_ary_entry( left, pos ) );
   }
   for ( pos = 0; pos < right_length; ++pos )
   {
      right_native[ pos ] = NUM2DBL( rb_ary_entry( right, pos ) );
   }

   for ( left_row = 0; left_row < left_rows; ++left_row )
   {
      for ( right_col = 0; right_col < right_cols; ++right_col )
      {
         for ( common = 0; common < left_cols; common += SSE_VECTOR_WIDTH )
         {
            for ( pos = 0; pos < SSE_VECTOR_WIDTH; ++pos )
            {
               input_index = common + pos;

               if ( input_index < left_cols )
               {
                  left_segment[ pos ]  = left_native[ left_row * left_cols + input_index ];
                  right_segment[ pos ] = right_native[ input_index * right_cols + right_col ];
               }
               else
               {
                  left_segment[ pos ]  = 0;
                  right_segment[ pos ] = 0;
               }
            }

            left_vec   = ( __m128i *)left_segment;
            right_vec  = ( __m128i *)right_segment;
            result_vec = _mm_mul_ps( *left_vec, *right_vec );

            _mm_store_si128( (__m128i*)result_segment, result_vec );
            for ( pos = 0; pos < SSE_VECTOR_WIDTH; ++pos )
            {
               if ( (common + pos) < left_cols )
               {
                  partial_native[ common + pos ] = result_segment[ pos ];
               }
            }
         }

         result_native[ left_row * right_cols + right_col ] = 0;
         temp = &result_native[ left_row * right_cols + right_col ];
         for ( common = 0; common < left_cols; ++common )
         {
            (*temp) += partial_native[ common ];
         }
      }
   }

   result = rb_ary_new2( result_length );
   for ( pos = 0; pos < result_length; ++pos )
   {
      rb_ary_push( result, DBL2NUM( result_native[ pos ] ) );
   }

   free( left_native );
   free( right_native );
   free( result_native );
   free( partial_native );

   return result;
}

