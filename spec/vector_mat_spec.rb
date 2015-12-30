begin
   require 'vector_sse'
rescue StandardError => e
   # vector_sse is not installed as a gem
   require File.join( '..', 'lib', 'vector_sse' )
end

RSpec.describe VectorSSE::Mat do

   describe "constructor" do

      it "raises exception on invalid type" do
         expect {
            VectorSSE::Mat.new( VectorSSE::Type::INVALID, 2, 1 )
         }.to raise_error ArgumentError, "invalid SSE matrix type for argument 0"
      end

      it "raises exception on invalid row dimension" do
         expect {
            VectorSSE::Mat.new( VectorSSE::Type::S32, 0, 1 )
         }.to raise_error ArgumentError, "row count must be greater than zero for argument 1"
      end

      it "raises exception on invalid column dimension" do
         expect {
            VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 0 )
         }.to raise_error ArgumentError, "column count must be greater than zero for argument 2"
      end

      it "initializes with correct type and dimensions" do
         mat = VectorSSE::Mat.new( VectorSSE::Type::S32, 10, 8 )
         expect( mat.type ).to eq( VectorSSE::Type::S32 )
         expect( mat.rows ).to eq( 10 )
         expect( mat.cols ).to eq( 8 )
      end

      it "initializes all elements to zero" do
         mat = VectorSSE::Mat.new( VectorSSE::Type::S32, 4, 2 )
         8.times do |index|
            expect( mat[ index ] ).to eq( 0 )
         end
      end

   end

   describe "attributes" do
      it "raises exception on attempt to access protected field" do
         expect {
            mat = VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 4 )
            protected_data = mat.data
         }.to raise_error NoMethodError
         expect {
            mat = VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 4 )
            mat.data = []
         }.to raise_error NoMethodError
      end
   end

   describe "read element" do

      mat = VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 4 )
      mat.fill([
         1, 2, 3, 4,
         5, 6, 7, 8
      ])

      it "raises exception on invalid row with 'at' reader" do
         expect {
            mat.at( -1, 0 )
         }.to raise_error "row index out of bounds"
         expect {
            mat.at( 2, 0 )
         }.to raise_error "row index out of bounds"
      end

      it "raises exception on invalid column with 'at' reader" do
         expect {
            mat.at( 0, -1 )
         }.to raise_error "column index out of bounds"
         expect {
            mat.at( 0, 4 )
         }.to raise_error "column index out of bounds"
      end

      it "raises exception on invalid index with '[]' reader" do
         expect {
            mat[ -1 ]
         }.to raise_error "index out of bounds"
         expect {
            mat[ 8 ]
         }.to raise_error "index out of bounds"
      end

      it "returns value from correct row/col location with 'at' reader" do
         expect( mat.at( 1, 2) ).to eq( 7 )
      end

      it "returns value from correct index with '[]' reader" do
         expect( mat[ 6 ] ).to eq( 7 )
      end
   end

   describe "write element" do
      mat = VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 4 )

      it "raises exception on invalid row with 'set' writer" do
         expect {
            mat.set( -1, 0, 0 )
         }.to raise_error "row index out of bounds"
         expect {
            mat.set( 2, 0, 0 )
         }.to raise_error "row index out of bounds"
      end

      it "raises exception on invalid column with 'set' writer" do
         expect {
            mat.set( 0, -1, 0 )
         }.to raise_error "column index out of bounds"
         expect {
            mat.set( 0, 4, 0 )
         }.to raise_error "column index out of bounds"
      end

      it "raises exception on invalid index with '[]=' write" do
         expect {
            mat[ -1 ] = 0
         }.to raise_error "index out of bounds"
         expect {
            mat[ 8 ] = 0
         }.to raise_error "index out of bounds"
      end

      it "sets value at correct row/col location with 'set' writer" do
         mat.set( 1, 2, 7 )
         expect( mat.at( 1, 2) ).to eq( 7 )
      end

      it "sets value at correct index with '[]=' writer" do
         mat[ 3 ] = 4
         expect( mat[ 3 ] ).to eq( 4 )
      end
   end

   describe "to string" do
      it "renders matrix as string" do
         mat = VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 4 )
         mat.fill([
            1, 2, 3, 4,
            5, 6, 7, 8
         ])

         expect( mat.to_s ).to eq( "|1 2 3 4|\n|5 6 7 8|\n" )
      end
   end

   describe "matrix addition and subtraction" do
      it "raises exception if the addends are not of equal size" do
         left = VectorSSE::Mat.new( VectorSSE::Type::S32, 3, 2 )
         left.fill([
            1, 2,
            3, 4,
            5, 6
         ])
         right = VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 1 )
         right.fill([
            1,
            2
         ])

         expect {
            left + right
         }.to raise_error ArgumentError, "matrix addition requires operands of equal size"

         expect {
            left - right
         }.to raise_error ArgumentError, "matrix subtraction requires operands of equal size"
      end

      it "returns correct sum" do
         left = VectorSSE::Mat.new( VectorSSE::Type::S32, 3, 2 )
         left.fill([
            1, 2,
            3, 4,
            5, 6
         ])
         right = VectorSSE::Mat.new( VectorSSE::Type::S32, 3, 2 )
         right.fill([
            1, 2,
            3, 4,
            5, 6
         ])

         result = left + right
         expect( result.type ).to eq( left.type )
         expect( result.rows ).to eq( left.rows )
         expect( result.cols ).to eq( left.cols )

         [  2,  4,
            6,  8,
           10, 12 ].each_with_index do |value,index|
            expect( result[ index ] ).to eq( value )
         end

         result = left - right
         expect( result.type ).to eq( left.type )
         expect( result.rows ).to eq( left.rows )
         expect( result.cols ).to eq( left.cols )

         [  0, 0,
            0, 0,
            0, 0 ].each_with_index do |value,index|
            expect( result[ index ] ).to eq( value )
         end
      end

      it "returns correct sum when added to self" do
         left = VectorSSE::Mat.new( VectorSSE::Type::S32, 3, 2 )
         left.fill([
            1, 2,
            3, 4,
            5, 6
         ])

         result = left + left
         expect( result.type ).to eq( left.type )
         expect( result.rows ).to eq( left.rows )
         expect( result.cols ).to eq( left.cols )

         [  2,  4,
            6,  8,
           10, 12 ].each_with_index do |value,index|
            expect( result[ index ] ).to eq( value )
         end
      end

      it "returns correct sum when added and assigned to self" do
         left = VectorSSE::Mat.new( VectorSSE::Type::S32, 3, 2 )
         left.fill([
            1, 2,
            3, 4,
            5, 6
         ])
         right = VectorSSE::Mat.new( VectorSSE::Type::S32, 3, 2 )
         right.fill([
            1, 2,
            3, 4,
            5, 6
         ])

         left += right
         expect( left.type ).to eq( VectorSSE::Type::S32 )
         expect( left.rows ).to eq( 3 )
         expect( left.cols ).to eq( 2 )

         [  2,  4,
            6,  8,
           10, 12 ].each_with_index do |value,index|
            expect( left[ index ] ).to eq( value )
         end
      end
   end

   describe "matrix multiplication" do
      it "raises exception for invalid argument" do
         left = VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 2 )
         right = "this is not a matrix"

         expect {
            left * right
         }.to raise_error ArgumentError, "expected argument of type VectorSSE::Mat for argument 0"
      end

      it "raises exception on invalid factor dimensions" do
         left = VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 2 )
         right = VectorSSE::Mat.new( VectorSSE::Type::S32, 1, 2 )

         expect {
            left * right
         }.to raise_error "invalid matrix dimensions"
      end

      it "returns correct signed 32-bit result" do
         left = VectorSSE::Mat.new( VectorSSE::Type::S32, 3, 2 )
         left.fill([
            1, 2,
            3, 4,
            5, 6
         ])
         right = VectorSSE::Mat.new( VectorSSE::Type::S32, 2, 1 )
         right.fill([
            1,
            2
         ])

         result = left * right
         expect( result.type ).to eq( left.type )
         expect( result.rows ).to eq( left.rows )
         expect( result.cols ).to eq( right.cols )

         [ 5, 11, 17 ].each_with_index do |value,index|
            expect( result[ index ] ).to eq( value )
         end
      end

      it "returns result with same type as left factor" do
         left = VectorSSE::Mat.new( VectorSSE::Type::S32, 3, 2 )
         left.fill([
            1, 2,
            3, 4,
            5, 6
         ])
         right = VectorSSE::Mat.new( VectorSSE::Type::F32, 2, 1 )
         right.fill([
            1.9,
            2.3
         ])

         result = left * right
         expect( result.type ).to eq( left.type )
         expect( result.rows ).to eq( left.rows )
         expect( result.cols ).to eq( right.cols )

         [ 5, 11, 17 ].each_with_index do |value,index|
            expect( result[ index ] ).to eq( value )
         end
      end

      it "returns correct float 32-bit product" do
         left = VectorSSE::Mat.new( VectorSSE::Type::F32, 3, 2 )
         left.fill([
            1.2, 2.3,
            3.4, 4.5,
            5.6, 6.7
         ])
         right = VectorSSE::Mat.new( VectorSSE::Type::F32, 2, 1 )
         right.fill([
            1.9,
            2.3
         ])

         result = left * right
         expect( result.type ).to eq( VectorSSE::Type::F32 )
         expect( result.rows ).to eq( left.rows )
         expect( result.cols ).to eq( right.cols )

         [  1.2 * 1.9 + 2.3 * 2.3,
            3.4 * 1.9 + 4.5 * 2.3,
            5.6 * 1.9 + 6.7 * 2.3 ].each_with_index do |value,index|
            expect( result[ index ] ).to be_within( 1e-6 ).of( value )
         end
      end

      it "returns sum of matrix and scalar" do
         original_values = [
            1.2, 2.3,
            3.4, 4.5,
            5.6, 6.7
         ]

         scalar_value = 1.1

         left = VectorSSE::Mat.new( VectorSSE::Type::F32, 3, 2 )
         left.fill( original_values )

         left -= scalar_value

         original_values.each_with_index do |value,index|
            expected_value = value - scalar_value
            expect( left[ index ] ).to be_within( 1e-6 ).of( expected_value )
         end
      end
   end

end