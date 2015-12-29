require 'rake/extensiontask'
require 'rake/testtask'

Rake::ExtensionTask.new "vector_sse" do |ext|
  ext.lib_dir = "lib/vector_sse"
end

Rake::TestTask.new do |t|
  t.libs << 'test'
end
