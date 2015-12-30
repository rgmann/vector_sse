require 'rake/extensiontask'
require 'rake/testtask'
require 'rspec/core/rake_task'

Rake::ExtensionTask.new "vector_sse" do |ext|
  ext.lib_dir = "lib/vector_sse"
end

RSpec::Core::RakeTask.new( :spec )
