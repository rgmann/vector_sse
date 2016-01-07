begin
   require 'vector_sse'
rescue StandardError => e
   # vector_sse is not installed as a gem
   require File.join( '..', 'lib', 'vector_sse' )
end

RSpec.describe VectorSSE::Array do

   describe "constructor" do
   end

   describe "element insertion" do

      it "rejects insertion of invalid data types" do
         arr = VectorSSE::Array.new( VectorSSE::Type::S32 )

         expect {
            arr << "not a valid data type"
         }.to raise_error ArgumentError

         expect {
            arr.insert( 0, [ 1, "not a valid data type" ] )
         }.to raise_error ArgumentError

         expect {
            arr[ 0 ] = "not a valid data type"
         }.to raise_error ArgumentError        
      end
   end

   describe "vector addition" do

      it "raises exception if right addend is shorter than the left addend in addition" do
         left = VectorSSE::Array.new( VectorSSE::Type::S32 )
         left.replace [ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ]
         right = VectorSSE::Array.new( VectorSSE::Type::S32 )
         right.replace [ 1, 2, 3, 4, 5 ]

         expect {
            result = left + right
         }.to raise_error "Vector lengths must be the same"
      end

      it "raises exception if right addend is shorter than the left addend in subtraction" do
         left = VectorSSE::Array.new( VectorSSE::Type::S32 )
         left.replace [ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ]
         right = VectorSSE::Array.new( VectorSSE::Type::S32 )
         right.replace [ 1, 2, 3, 4, 5 ]

         expect {
            result = left - right
         }.to raise_error "Vector lengths must be the same"
      end

      it "raises exception if right addend is longer than the left addend in addition" do
         left = VectorSSE::Array.new( VectorSSE::Type::S32 )
         left.replace [ 1, 2, 3, 4, 5 ]
         right = VectorSSE::Array.new( VectorSSE::Type::S32 )
         right.replace [ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ]

         expect {
            result = left + right
         }.to raise_error "Vector lengths must be the same"
      end

      it "raises exception if right addend is longer than the left addend in subtraction" do
         left = VectorSSE::Array.new( VectorSSE::Type::S32 )
         left.replace [ 1, 2, 3, 4, 5 ]
         right = VectorSSE::Array.new( VectorSSE::Type::S32 )
         right.replace [ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ]

         expect {
            result = left - right
         }.to raise_error "Vector lengths must be the same"
      end

      it "returns difference between vectors" do
         left = VectorSSE::Array.new( VectorSSE::Type::S32 )
         left.replace [ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ]
         right = VectorSSE::Array.new( VectorSSE::Type::S32 )
         right.replace [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]

         result = left - right
         expect( result.type ).to eq( left.type )
         expect( result.length ).to eq( left.length )

         [ 9, 7, 5, 3, 1, -1, -3, -5, -7, -9 ].each_with_index do |value,index|
            expect( result[ index ] ).to eq( value )
         end
      end

      it "returns difference after subtracting scalar Fixnum" do
         left = VectorSSE::Array.new( VectorSSE::Type::S32 )
         left.replace [ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ]

         left -= 2

         expect( left.type ).to eq( VectorSSE::Type::S32 )
         expect( left.length ).to eq( 10 )

         [ 8, 7, 6, 5, 4, 3, 2, 1, 0, -1 ].each_with_index do |value,index|
            expect( left[ index ] ).to eq( value )
         end
      end

      it "returns difference after subtracting scalar Float" do

         values = [ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ]
         scalar_value = 2.8

         left = VectorSSE::Array.new( VectorSSE::Type::F32 )
         left.replace values

         left -= scalar_value

         expect( left.type ).to eq( VectorSSE::Type::F32 )
         expect( left.length ).to eq( 10 )

         values.each_with_index do |value,index|
            expected_value = value - scalar_value
            expect( left[ index ] ).to be_within( 1e-6 ).of( expected_value )
         end
      end
   end

   describe "scalar vector multiplication" do

      it "performs scalar multiplication when right factor is scalar integer" do
         data = [ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ]
         scalar_value = -2

         left = VectorSSE::Array.new( VectorSSE::Type::S32 )
         left.replace data

         left *= scalar_value

         expect( left.length ).to eq( data.length )
         data.each_with_index do |value,index|
            expect( left[ index ] ).to eq( value * scalar_value )
         end

      end

      it "performs scalar multiplication when right factor is 64-bit scalar integer" do
         data = [ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ]
         scalar_value = -2

         left = VectorSSE::Array.new( VectorSSE::Type::S64 )
         left.replace data

         left *= scalar_value

         expect( left.length ).to eq( data.length )
         data.each_with_index do |value,index|
            expect( left[ index ] ).to eq( value * scalar_value )
         end

      end

      it "performs scalar multiplication when right factor is scalar float" do
         data = [ 10.1, 9.2, 8.3, 7.4, 6.5, 5.6, 4.7, 3.8, 2.9, 1.93 ]
         scalar_value = -2.2

         left = VectorSSE::Array.new( VectorSSE::Type::F32 )
         left.replace data

         left *= scalar_value

         expect( left.length ).to eq( data.length )
         data.each_with_index do |value,index|
            expect( left[ index ] ).to be_within( 2e-6 ).of( value * scalar_value )
         end

      end

      it "performs scalar multiplication when right factor is scalar double" do
         data = [ 10.1, 9.2, 8.3, 7.4, 6.5, 5.6, 4.7, 3.8, 2.9, 1.93 ]
         scalar_value = -2.2

         left = VectorSSE::Array.new( VectorSSE::Type::F64 )
         left.replace data

         left *= scalar_value

         expect( left.length ).to eq( data.length )
         data.each_with_index do |value,index|
            expect( left[ index ] ).to be_within( 1e-6 ).of( value * scalar_value )
         end

      end

   end

end
