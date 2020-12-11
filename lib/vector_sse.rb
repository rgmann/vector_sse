#
# Copyright (c) 2015, Robert Glissmann
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# %% license-end-token %%
# 
# Author: Robert.Glissmann@gmail.com (Robert Glissmann)
# 
# 

bin_root = File.join( File.dirname( __FILE__ ), 'vector_sse' )
require File.join( bin_root, 'vector_sse.so' )


module VectorSSE

   VERSION = "0.0.3"

   module Type
      S32 = 0
      S64 = 1
      F32 = 4
      F64 = 5
      INVALID = -1
   end

   def self.valid_type( type )
      [ Type::S32, Type::S64, Type::F32, Type::F64 ].include?( type )
   end


   class Mat

      MIN_ROW_COL_COUNT = 1

      attr_reader :type
      attr_reader :rows
      attr_reader :cols

      def initialize( type, rows, cols, data=nil )

         if VectorSSE::valid_type( type )
            @type = type
         else
            raise ArgumentError.new( "invalid SSE matrix type for argument 0" )
         end

         if rows < MIN_ROW_COL_COUNT
            raise ArgumentError.new( "row count must be greater than zero for argument 1" )
         end

         if cols < MIN_ROW_COL_COUNT
            raise ArgumentError.new( "column count must be greater than zero for argument 2" )
         end

         if data && ( data.class != ::Array )
            raise ArgumentError.new( "expected value of type Array for argument 3" )
         end

         @rows = rows
         @cols = cols
         @linear_size = @rows * @cols

         @data = ::Array.new( @linear_size, 0 )

         fill( data ) if data
      end

      def at( row, col )
         valid_row_col( row, col )
         @data[ linear_index( row, col ) ]
      end

      def set( row, col, val )
         valid_row_col( row, col )
         valid_data_type( val )
         @data[ linear_index( row, col ) ] = val
      end

      def fill( data )
         if data.length != @rows * @cols
            raise ArgumentError.new( "size does not match matrix size" )
         end

         data.each do |value|
            valid_data_type( value )
         end

         @data = data
      end

      def []( pos )
         valid_linear_index( pos )
         @data[ pos ]
      end

      def []=( pos, val )
         valid_linear_index( pos )
         valid_data_type( val )
         @data[ pos ] = val
      end

      def to_s
         text = ""
         @rows.times do |r|
            vals = []
            @cols.times do |c|
               vals << @data[r*@cols + c]
            end
            text << "|#{vals.join(' ')}|\n"
         end
         text
      end

      def *( other )

         scalar_mul = false

         if [ Integer, Float ].include? other.class

            scalar_mul = true
            scalar_value = other
            other = Mat.new( @type, @rows, @cols )
            other.data.replace( ::Array.new( @linear_size, scalar_value ) )

         elsif other.class == self.class

            if @cols != other.rows
               raise "invalid matrix dimensions"
            end

         else
            raise ArgumentError.new(
               "expected argument of type #{self.class} for argument 0" )
         end
         

         result = Mat.new( @type, @rows, other.cols )

         case @type
         when Type::S32
            if scalar_mul
               result.data.replace( VectorSSE::vec_mul_s32( self.data, other.data ) )
            else
               result.data.replace( VectorSSE::mul_s32(
                  @data, @rows, @cols, other.data, other.rows, other.cols ) )
            end
         when Type::S64
            if scalar_mul
               result.data.replace( VectorSSE::vec_mul_s32( self.data, other.data ) )
            else
               result.data.replace( VectorSSE::mul_s64(
                  @data, @rows, @cols, other.data, other.rows, other.cols ) )
            end
         when Type::F32
            if scalar_mul
               result.data.replace( VectorSSE::vec_mul_f32( self.data, other.data ) )
            else
               result.data.replace( VectorSSE::mul_f32(
                  @data, @rows, @cols, other.data, other.rows, other.cols ) )
            end
         when Type::F64
            if scalar_mul
               result.data.replace( VectorSSE::vec_mul_f64( self.data, other.data ) )
            else
               result.data.replace( VectorSSE::mul_f64(
                  @data, @rows, @cols, other.data, other.rows, other.cols ) )
            end
         end

         result
      end

      def +( other )

         if [ Integer, Float ].include? other.class

            scalar_value = other
            other = Mat.new( @type, @rows, @cols )
            other.data.replace( ::Array.new( @linear_size, scalar_value ) )

         elsif other.class == self.class

            if ( @rows != other.rows ) || ( @cols != other.cols )
               raise ArgumentError.new(
                  "matrix addition requires operands of equal size")
            end

         else

            raise ArgumentError.new(
               "expect argument of type #{self.class}, Integer, or Float for argument 0" )

         end

         result = Mat.new( @type, @rows, @cols )

         case @type
         when Type::S32
            result.data.replace( VectorSSE::add_s32( self.data, other.data ) )
         when Type::S64
            result.data.replace( VectorSSE::add_s64( self.data, other.data ) )
         when Type::F32
            result.data.replace( VectorSSE::add_f32( self.data, other.data ) )
         when Type::F64
            result.data.replace( VectorSSE::add_f64( self.data, other.data ) )
         end

         result
      end

      def -( other )

         if [ Integer, Float ].include? other.class

            scalar_value = other
            other = Mat.new( @type, @rows, @cols )
            other.data = ::Array.new( @linear_size, scalar_value )

         elsif other.class == self.class

            if ( @rows != other.rows ) || ( @cols != other.cols )
               raise ArgumentError.new(
                  "matrix subtraction requires operands of equal size")
            end

         else

            raise ArgumentError.new(
               "expect argument of type #{self.class}, Integer, or Float for argument 0" )

         end

         result = Mat.new( @type, @rows, @cols )

         case @type
         when Type::S32
            result.data.replace( VectorSSE::sub_s32( self.data, other.data ) )
         when Type::S64
            result.data.replace( VectorSSE::sub_s64( self.data, other.data ) )
         when Type::F32
            result.data.replace( VectorSSE::sub_f32( self.data, other.data ) )
         when Type::F64
            result.data.replace( VectorSSE::sub_f64( self.data, other.data ) )
         end

         result
      end

      def transpose
         raise "unimplemented"
      end

      def reshape( rows, cols )
         raise "unimplemented"
      end


      protected


      def linear_index( row, col )

         row * @cols + col

      end

      def valid_linear_index( pos )

         if ( pos < 0 ) || ( pos >= @linear_size )
            raise IndexError.new( "index out of bounds" )
         end

      end

      def valid_row_col( row, col )

         if ( row < 0 ) || ( row >= @rows )
            raise IndexError.new( "row index out of bounds" )
         end

         if ( col < 0 ) || ( col >= @cols )
            raise IndexError.new( "column index out of bounds" )
         end

      end

      def valid_data_type( value )

         unless [ Integer, Float ].include? value.class
            raise ArgumentError.new( "expected argument of type Integer or Float" )
         end

      end

      attr_accessor :data
      
   end
   Matrix = Mat


   class Array < Array

      attr_reader :type

      def initialize( type, size=0, val=nil )
         super( size, val )

         if VectorSSE::valid_type( type )
            @type = type
         else
            raise "invalid SSE vector type"
         end
      end

      def <<( value )
         unless [ Integer, Float ].include? value.class
            raise ArgumentError.new(
               "expected argument of type Integer or Float for argument 0" )
         end
         super( value )
      end

      def insert( index, *values )
         values.each_with_index do |value,arg_index|
            unless [ Integer, Float ].include? value.class
               raise ArgumentError.new(
                  "expected argument of type Integer or Float for argument #{arg_index}" )
            end
         end
         super( index, values )
      end

      def []=( index, value )
         unless [ Integer, Float ].include? value.class
            raise ArgumentError.new(
               "expected argument of type Integer or Float for argument 1" )
         end
         super( index, value )
      end
 
      # Note:
      # This method replaces the base class implementation of '+', which
      # performs concatenation. To concatenate, see #concat.
      #
      def +( other )

         if [ Integer, Float ].include? other.class

            other = ::Array.new( self.length, other )

         elsif other.class != self.class

            raise ArgumentError.new(
               "expect argument of type #{self.class}, Integer, or Float for argument 0" )

         end

         result = self.class.new( @type )

         case @type
         when Type::S32
            result.replace( VectorSSE::add_s32( self, other ) )
         when Type::S64
            result.replace( VectorSSE::add_s64( self, other ) )
         when Type::F32
            result.replace( VectorSSE::add_f32( self, other ) )
         when Type::F64
            result.replace( VectorSSE::add_f64( self, other ) )
         end

         result
      end

      # Note:
      # This method replaces the base class implementation of '-', which
      # removes items that are found in 'other'.
      #
      def -( other )

         if [ Integer, Float ].include? other.class
            other = ::Array.new( self.length, other )
         elsif other.class != self.class
            raise ArgumentError.new(
               "expected argument of type #{self.class}, Integer, or Float for argument 0" )
         end

         result = self.class.new( @type )

         case @type
         when Type::S32
            result.replace( VectorSSE::sub_s32( self, other ) )
         when Type::S64
            result.replace( VectorSSE::sub_s64( self, other ) )
         when Type::F32
            result.replace( VectorSSE::sub_f32( self, other ) )
         when Type::F64
            result.replace( VectorSSE::sub_f64( self, other ) )
         end

         result
      end

      def sum
         sum_result = 0

         case @type
         when Type::S32
            sum_result = VectorSSE::sum_s32( self )
         when Type::S64
            sum_result = VectorSSE::sum_s64( self )
         when Type::F32
            sum_result = VectorSSE::sum_f32( self )
         when Type::F64
            sum_result = VectorSSE::sum_f64( self )
         else
            raise "invalid SSE vector type"
         end

         sum_result
      end

      def *( other )

         unless [ Integer, Float ].include? other.class

            raise ArgumentError.new( "expected argument of type Float or Integer for argument 0" )

         end

         other  = self.class.new( @type, self.length, other )
         result = self.class.new( @type )

         case @type
         when Type::S32
            result.replace( VectorSSE::vec_mul_s32( self, other ) )
         when Type::S64
            result.replace( VectorSSE::vec_mul_s64( self, other ) )
         when Type::F32
            result.replace( VectorSSE::vec_mul_f32( self, other ) )
         when Type::F64
            result.replace( VectorSSE::vec_mul_f64( self, other ) )
         end

         result
      end

   end
   Arr = Array


end # module VectorSSE

