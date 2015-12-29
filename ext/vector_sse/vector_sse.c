// Include the Ruby headers and goodies
#include <emmintrin.h>
#include <stdio.h>
#include "ruby.h"

#include "vector_sse_add.h"
#include "vector_sse_sum.h"
#include "vector_sse_mul.h"
#include "vector_sse_vec_mul.h"


struct vector_sse_result {
   VALUE result;
   VALUE overflow;
};

// Defining a space for information and references about the module to be stored internally
VALUE VectorSse = Qnil;

// Prototype for the initialization method - Ruby calls this, not you
void Init_vector_sse();


// The initialization method for this module
void Init_vector_sse() {

   VectorSse = rb_define_module("VectorSse");

   rb_define_singleton_method( VectorSse, "add_s32", method_vec_add_s32, 2 );
   rb_define_singleton_method( VectorSse, "add_s64", method_vec_add_s64, 2 );
   rb_define_singleton_method( VectorSse, "add_f32", method_vec_add_f32, 2 );
   rb_define_singleton_method( VectorSse, "add_f64", method_vec_add_f64, 2 );

   rb_define_singleton_method( VectorSse, "sub_s32", method_vec_sub_s32, 2 );
   rb_define_singleton_method( VectorSse, "sub_s64", method_vec_sub_s64, 2 );
   rb_define_singleton_method( VectorSse, "sub_f32", method_vec_sub_f32, 2 );
   rb_define_singleton_method( VectorSse, "sub_f64", method_vec_sub_f64, 2 );

   rb_define_singleton_method( VectorSse, "sum_s32", method_vec_sum_s32, 1 );
   rb_define_singleton_method( VectorSse, "sum_s64", method_vec_sum_s64, 1 );
   rb_define_singleton_method( VectorSse, "sum_f32", method_vec_sum_f32, 1 );
   rb_define_singleton_method( VectorSse, "sum_f64", method_vec_sum_f64, 1 );

   rb_define_singleton_method( VectorSse, "mul_s32", method_mat_mul_s32, 6 );
   rb_define_singleton_method( VectorSse, "mul_s64", method_mat_mul_s64, 6 );
   rb_define_singleton_method( VectorSse, "mul_f32", method_mat_mul_f32, 6 );

   rb_define_singleton_method( VectorSse, "vec_mul_s32", method_vec_mul_s32, 2 );
   rb_define_singleton_method( VectorSse, "vec_mul_f32", method_vec_mul_f32, 2 );
   rb_define_singleton_method( VectorSse, "vec_mul_f64", method_vec_mul_f64, 2 );
}

