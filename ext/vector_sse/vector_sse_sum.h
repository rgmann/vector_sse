#ifndef  VECTOR_SSE_SUM_H
#define  VECTOR_SSE_SUM_H

#include "ruby.h"

VALUE method_vec_sum_s32( VALUE self, VALUE vector );
VALUE method_vec_sum_s64( VALUE self, VALUE vector );
VALUE method_vec_sum_f32( VALUE self, VALUE vector );
VALUE method_vec_sum_f64( VALUE self, VALUE vector );

#endif // VECTOR_SSE_SUM_H