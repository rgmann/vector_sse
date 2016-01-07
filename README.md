## Welcome to VectorSSE ##

VectorSSE is a Ruby gem that uses x86 Streaming SIMD Extensions (SSE) to accelerate
array and matrix computations. SIMD instructions perform a single operation
on a vector of data rather than on a single value. Vector operations can
improve the performance of algorithms that exhibit data-level parallelism.

A simple example:
Let us say you need to compute the sum of an array containing 1000 floating
point numbers. One approach is to loop through the array and consecutively add
each element to a running sum. Another approach is to break the problem
into a set of smaller, independent computations that can be performed in parallel
with SIMD instructions. For example, we can break the 1000-element array into
four 250-element arrays and use SIMD extensions to find four sums in parallel.
Of course, this leaves us with four separate sums, so we must add these using
normal, non-SSE instructions to yield the overall sum of the 1000-element array.


## Install the gem ##

Install it with [RubyGems](https://rubygems.org/)

    gem install vector_sse

or add this to your Gemfile if you use [Bundler](http://gembundler.com/):

    gem "vector_sse"


## Getting Started ##

The VectorSSE gem defines two data types: the Array class, which inherits from
the core Array class, and the Matrix class. Unlike typical Ruby containers, the
Array and Matrix classes are intended to store a homogeneous data type. At this
time, the supported data types include signed 32 and 64-bit signed integers,
32-bit floating point, and double-precision floating point. The type is
identified when the Array or Matrix is constructed so that all operations can
use the appropriate implementation.


### Example: Multiply two matrices ###

     require 'vector_sse'

     left = VectorSSE::Matrix.new( VectorSSE::Type::F32, 4, 4, [
           1.2,  2.3,  3.4,  4.5,
           5.6,  6.7,  7.8,  8.9,
           9.05, 10.9, 11.85, 12.2,
           13.43, 14.85, 15.67, 16.5
     ])
     right = VectorSSE::Matrix.new( VectorSSE::Type::F32, 4, 2, [
           1,  2,
           5,  6,
           9, 10,
          13, 14
     ])

     product = left * right


### Example: Scale a matrix by a scalar value ###

     require 'vector_sse'

     left = VectorSSE::Matrix.new( VectorSSE::Type::F32, 4, 4, [
          1.2,  2.3,  3.4,  4.5,
          5.6,  6.7,  7.8,  8.9,
          9.05, 10.9, 11.85, 12.2,
          13.43, 14.85, 15.67, 16.5
     ])

     product = left * 3.14


### Example: Subtract Arrays and find the sum of the elements of an Array ###

     require 'vector_sse'

     # Initialize a four element integer array
     left = VectoSSE::Array.new( VectorSSE::Type::S32, 10 )
     left.fill([
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10
     ])
     right = VectoSSE::Array.new( VectorSSE::Type::S32, 10 )
     right.fill([
          10, 9, 8, 7, 6, 5, 4, 3, 2, 1
     ])

     # Subtract the arrays
     result = left - right

     # Get the sum of the elements of an array
     sum = left.sum


## License and copyright ##

VectorSSE is released under the BSD License.

Copyright: (C) 2015 by Robert Glissmann. All Rights Reserved.
