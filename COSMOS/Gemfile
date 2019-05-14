# encoding: ascii-8bit

source 'https://rubygems.org'

gem 'ruby-termios', '~> 0.9' if RbConfig::CONFIG['target_os'] !~ /mswin|mingw|cygwin/i and RUBY_ENGINE=='ruby'
if ENV['COSMOS_DEVEL']
  gem 'cosmos', :path => ENV['COSMOS_DEVEL']
else
  gem 'cosmos'
end

# Uncomment this line to add DART dependencies to your main Gemfile
# instance_eval File.read(File.join(__dir__, 'config/dart/Gemfile'))
