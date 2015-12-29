#ifndef  VECTOR_SSE_VEC_MUL_H
#define  VECTOR_SSE_VEC_MUL_H

#include <ruby.h>

VALUE method_vec_mul_s32( VALUE self, VALUE left, VALUE right );
VALUE method_vec_mul_f32( VALUE self, VALUE left, VALUE right );
VALUE method_vec_mul_f64( VALUE self, VALUE left, VALUE right );

#endif  // VECTOR_SSE_VEC_MUL_H
