#ifndef  VECTOR_SSE_ADD_H
#define  VECTOR_SSE_ADD_H

#include "ruby.h"

VALUE method_vec_add_s32( VALUE self, VALUE left, VALUE right );
VALUE method_vec_add_s64( VALUE self, VALUE left, VALUE right );
VALUE method_vec_add_f32( VALUE self, VALUE left, VALUE right );
VALUE method_vec_add_f64( VALUE self, VALUE left, VALUE right );

VALUE method_vec_sub_s32( VALUE self, VALUE left, VALUE right );
VALUE method_vec_sub_s64( VALUE self, VALUE left, VALUE right );
VALUE method_vec_sub_f32( VALUE self, VALUE left, VALUE right );
VALUE method_vec_sub_f64( VALUE self, VALUE left, VALUE right );

#endif  // VECTOR_SSE_ADD_H