Gem::Specification.new do |s|
   s.name        = 'vector_sse'
   s.version     = '0.0.0'
   s.date        = '2010-04-28'
   s.summary     = "SIMD vector and matrix operations"
   s.description = "A gem for performing SIMD arithmetic, via x86 SSE, in Ruby"
   s.authors     = [ "Robert Glissmann" ]
   s.email       = 'Robert.Glissmann@gmail.com'
   s.files       = [ "lib/vector_sse.rb" ]
   s.extensions  = %w[ext/vector_sse/extconf.rb]
   s.license     = 'BSD'
end
