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


// Include the Ruby headers and goodies
#include <emmintrin.h>
#include <stdio.h>
#include "ruby.h"

#include "vector_sse_add.h"
#include "vector_sse_sum.h"
#include "vector_sse_mul.h"
#include "vector_sse_vec_mul.h"

// TODO:
struct vector_sse_result {
   VALUE result;
   VALUE overflow;
};

// Defining a space for information and references about the module to be stored internally
VALUE VectorSSE = Qnil;

// Prototype for the initialization method - Ruby calls this, not you
void Init_vector_sse();


// The initialization method for this module
void Init_vector_sse() {

   VectorSSE = rb_define_module("VectorSSE");

   rb_define_singleton_method( VectorSSE, "add_s32", method_vec_add_s32, 2 );
   rb_define_singleton_method( VectorSSE, "add_s64", method_vec_add_s64, 2 );
   rb_define_singleton_method( VectorSSE, "add_f32", method_vec_add_f32, 2 );
   rb_define_singleton_method( VectorSSE, "add_f64", method_vec_add_f64, 2 );

   rb_define_singleton_method( VectorSSE, "sub_s32", method_vec_sub_s32, 2 );
   rb_define_singleton_method( VectorSSE, "sub_s64", method_vec_sub_s64, 2 );
   rb_define_singleton_method( VectorSSE, "sub_f32", method_vec_sub_f32, 2 );
   rb_define_singleton_method( VectorSSE, "sub_f64", method_vec_sub_f64, 2 );

   rb_define_singleton_method( VectorSSE, "sum_s32", method_vec_sum_s32, 1 );
   rb_define_singleton_method( VectorSSE, "sum_s64", method_vec_sum_s64, 1 );
   rb_define_singleton_method( VectorSSE, "sum_f32", method_vec_sum_f32, 1 );
   rb_define_singleton_method( VectorSSE, "sum_f64", method_vec_sum_f64, 1 );

   rb_define_singleton_method( VectorSSE, "mul_s32", method_mat_mul_s32, 6 );
   rb_define_singleton_method( VectorSSE, "mul_s64", method_mat_mul_s64, 6 );
   rb_define_singleton_method( VectorSSE, "mul_f32", method_mat_mul_f32, 6 );

   rb_define_singleton_method( VectorSSE, "vec_mul_s32", method_vec_mul_s32, 2 );
   rb_define_singleton_method( VectorSSE, "vec_mul_s64", method_vec_mul_s64, 2 );
   rb_define_singleton_method( VectorSSE, "vec_mul_f32", method_vec_mul_f32, 2 );
   rb_define_singleton_method( VectorSSE, "vec_mul_f64", method_vec_mul_f64, 2 );
}

