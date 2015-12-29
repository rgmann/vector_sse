bin_root = File.join( File.dirname( __FILE__ ), 'vector_sse' )
require File.join( bin_root, 'vector_sse.so' )


module VectorSse

   VERSION = "1.0"

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

      attr_reader :overflow

      def initialize( type, rows, cols )

         if VectorSse::valid_type( type )
            @type = type
         else
            raise "invalid SSE matrix type"
         end

         if rows < MIN_ROW_COL_COUNT
            raise "row count must be greater than zero"
         end

         if cols < MIN_ROW_COL_COUNT
            raise "column count must be greater than zero"
         end

         @rows = rows
         @cols = cols
         @linear_size = @rows * @cols

         @data = Array.new( @linear_size, 0 )
      end

      def at( row, col )
         valid_row_col( row, col )
         @data[ linear_index( row, col ) ]
      end

      def set( row, col, val )
         valid_row_col( row, col )
         @data[ linear_index( row, col ) ] = val
      end

      def fill( data )
         if data.length != @rows * @cols
            raise "size does not match matrix size"
         end
         @data = data
      end

      def []( pos )
         valid_linear_index( pos )
         @data[ pos ]
      end

      def []=( pos, val )
         valid_linear_index( pos )
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

         if [ Fixnum, Float ].include? other.class

            scalar_mul = true
            scalar_value = other
            other = Mat.new( @type, @rows, @cols )
            other.data.replace( Array.new( @linear_size, scalar_value ) )

         elsif other.class != self.class

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
               result.data.replace( VectorSse::vec_mul_s32( self.data, other.data ) )
            else
               result.data.replace( VectorSse::mul_s32(
                  @data, @rows, @cols, other.data, other.rows, other.cols ) )
            end
         when Type::S64
            if scalar_mul
            else
               result.data.replace( VectorSse::mul_s64(
                  @data, @rows, @cols, other.data, other.rows, other.cols ) )
            end
         when Type::F32
            if scalar_mul
               result.data.replace( VectorSse::vec_mul_f32( self.data, other.data ) )
            else
               result.data.replace( VectorSse::mul_f32(
                  @data, @rows, @cols, other.data, other.rows, other.cols ) )
            end
         when Type::F64
            if scalar_mul
               result.data.replace( VectorSse::vec_mul_f64( self.data, other.data ) )
            else
               result.data.replace( VectorSse::mul_f64(
                  @data, @rows, @cols, other.data, other.rows, other.cols ) )
            end
         end

         result
      end

      def +( other )

         if [ Fixnum, Float ].include? other.class

            scalar_value = other
            other = Mat.new( @type, @rows, @cols )
            other.data.replace( Array.new( @linear_size, scalar_value ) )

         elsif other.class == self.class

            if ( @rows != other.rows ) || ( @cols != other.cols )
               raise ArgumentError.new(
                  "matrix addition requires operands of equal size")
            end

         else

            raise ArgumentError.new(
               "expect argument of type #{self.class}, Fixnum, or Float for argument 0" )

         end

         result = Mat.new( @type, @rows, @cols )

         case @type
         when Type::S32
            result.data.replace( VectorSse::add_s32( self.data, other.data ) )
         when Type::S64
            result.data.replace( VectorSse::add_s64( self.data, other.data ) )
         when Type::F32
            result.data.replace( VectorSse::add_f32( self.data, other.data ) )
         when Type::F64
            result.data.replace( VectorSse::add_f64( self.data, other.data ) )
         end

         result
      end

      def -( other )

         if [ Fixnum, Float ].include? other.class

            scalar_value = other
            other = Mat.new( @type, @rows, @cols )
            other.data = Array.new( @linear_size, scalar_value )

         elsif other.class == self.class

            if ( @rows != other.rows ) || ( @cols != other.cols )
               raise ArgumentError.new(
                  "matrix subtraction requires operands of equal size")
            end

         else

            raise ArgumentError.new(
               "expect argument of type #{self.class}, Fixnum, or Float for argument 0" )

         end

         result = Mat.new( @type, @rows, @cols )

         case @type
         when Type::S32
            result.data.replace( VectorSse::sub_s32( self.data, other.data ) )
         when Type::S64
            result.data.replace( VectorSse::sub_s64( self.data, other.data ) )
         when Type::F32
            result.data.replace( VectorSse::sub_f32( self.data, other.data ) )
         when Type::F64
            result.data.replace( VectorSse::sub_f64( self.data, other.data ) )
         end

         result
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

      attr_accessor :data
      
   end
   Matrix = Mat


   class Vec < Array

      attr_reader :type

      def initialize( type, size=0, val=nil )
         super( size, val )

         if VectorSse::valid_type( type )
            @type = type
         else
            raise "invalid SSE vector type"
         end
      end
 
      # Note:
      # This method replaces the base class implementation of '+', which
      # performs concatenation. To concatenate, see #concat.
      #
      def +( other )

         if [ Fixnum, Float ].include? other.class

            other = Array.new( self.length, other )

         elsif other.class != self.class

            raise ArgumentError.new(
               "expect argument of type #{self.class}, Fixnum, or Float for argument 0" )

         end

         result = Vector.new( @type )

         case @type
         when Type::S32
            result.replace( VectorSse::add_s32( self, other ) )
         when Type::S64
            result.replace( VectorSse::add_s64( self, other ) )
         when Type::F32
            result.replace( VectorSse::add_f32( self, other ) )
         when Type::F64
            result.replace( VectorSse::add_f64( self, other ) )
         end

         result
      end

      # Note:
      # This method replaces the base class implementation of '-', which
      # removes items that are found in 'other'.
      #
      def -( other )

         if [ Fixnum, Float ].include? other.class
            other = Array.new( self.length, other )
         elsif other.class != self.class
            raise ArgumentError.new(
               "expect argument of type #{self.class}, Fixnum, or Float for argument 0" )
         end

         result = Vector.new( @type )

         case @type
         when Type::S32
            result.replace( VectorSse::sub_s32( self, other ) )
         when Type::S64
            result.replace( VectorSse::sub_s64( self, other ) )
         when Type::F32
            result.replace( VectorSse::sub_f32( self, other ) )
         when Type::F64
            result.replace( VectorSse::sub_f64( self, other ) )
         end

         result
      end

      def sum
         sum_result = 0

         case @type
         when Type::S32
            sum_result = VectorSse::sum_s32( self )
         when Type::S64
            sum_result = VectorSse::sum_s64( self )
         when Type::F32
            sum_result = VectorSse::sum_f32( self )
         when Type::F64
            sum_result = VectorSse::sum_f64( self )
         else
            raise "invalid SSE vector type"
         end

         sum_result
      end

      def *( other )

         unless [ Fixnum, Float ].include? other.class
         end

         other = Vector.new( @type, self.length, other )
         result = Vector.new( @type )

         case @type
         when Type::S32
            result.replace( VectorSse::vec_mul_s32( self, other ) )
         when Type::F32
            result.replace( VectorSse::vec_mul_f32( self, other ) )
         when Type::F64
            result.replace( VectorSse::vec_mul_f64( self, other ) )
         end

         result
      end

   end
   Vector = Vec


end # module VectorSse

