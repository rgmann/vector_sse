#ifndef  VECTOR_SSE_MUL_H
#define  VECTOR_SSE_MUL_H

#include "ruby.h"

VALUE method_mat_mul_s32( VALUE self, VALUE left, VALUE left_rows_rb, VALUE left_cols_rb, VALUE right, VALUE right_rows_rb, VALUE right_cols_rb );
VALUE method_mat_mul_s64( VALUE self, VALUE left, VALUE left_rows_rb, VALUE left_cols_rb, VALUE right, VALUE right_rows_rb, VALUE right_cols_rb );
VALUE method_mat_mul_f32( VALUE self, VALUE left, VALUE left_rows_rb, VALUE left_cols_rb, VALUE right, VALUE right_rows_rb, VALUE right_cols_rb );
VALUE method_mat_mul_f64( VALUE self, VALUE left, VALUE left_rows_rb, VALUE left_cols_rb, VALUE right, VALUE right_rows_rb, VALUE right_cols_rb );

#endif // VECTOR_SSE_MUL_H