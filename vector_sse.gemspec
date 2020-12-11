# -*- encoding: utf-8 -*-
require "vector_sse/version"

Gem::Specification.new do |s|
   s.name        = 'vector_sse'
   s.version     = VectorSSE::VERSION.dup
   s.date        = Time.now.to_date.strftime('%Y-%m-%d')
   s.summary     = "SIMD accelerated vector and matrix operations"
   s.description = "VectorSse employs x86 Streaming SIMD Extensions (SSE), v3 or greater, to accelerate basic vector and matrix computations in Ruby."
   s.authors     = [ "Robert Glissmann" ]
   s.email       = 'Robert.Glissmann@gmail.com'
   s.files       = `git ls-files`.split("\n")
   s.extensions  = %w[ext/vector_sse/extconf.rb]
   s.licenses    = ['BSD']
   s.homepage    = 'https://github.com/rgmann/vector_sse'

   s.add_development_dependency 'rake-compiler', '~> 0.9.5'
   s.add_development_dependency 'rspec', '~> 3.1'
end
