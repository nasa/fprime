# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'cosmos'
if ENV['DART_USERNAME']
  require 'cosmos/dart/config/application'
  Rails.application.load_tasks
end

def create_crc_file(official)
  count = 0
  ignore = [
    # Add filenames here if you don't want them to be CRCed
    "tools/mac/CmdExtractor.app/Contents/MacOS/CmdExtractor",
    "tools/mac/CmdSender.app/Contents/MacOS/CmdSender",
    "tools/mac/CmdTlmServer.app/Contents/MacOS/CmdTlmServer",
    "tools/mac/DataViewer.app/Contents/MacOS/DataViewer",
    "tools/mac/HandbookCreator.app/Contents/MacOS/HandbookCreator",
    "tools/mac/Launcher.app/Contents/MacOS/Launcher",
    "tools/mac/LimitsMonitor.app/Contents/MacOS/LimitsMonitor",
    "tools/mac/OpenGLBuilder.app/Contents/MacOS/OpenGLBuilder",
    "tools/mac/PacketViewer.app/Contents/MacOS/PacketViewer",
    "tools/mac/Replay.app/Contents/MacOS/Replay",
    "tools/mac/ScriptRunner.app/Contents/MacOS/ScriptRunner",
    "tools/mac/TableManager.app/Contents/MacOS/TableManager",
    "tools/mac/TestRunner.app/Contents/MacOS/TestRunner",
    "tools/mac/TlmExtractor.app/Contents/MacOS/TlmExtractor",
    "tools/mac/TlmGrapher.app/Contents/MacOS/TlmGrapher",
    "tools/mac/TlmViewer.app/Contents/MacOS/TlmViewer",
  ]
  # Create the crc.txt file
  crc = Cosmos::Crc32.new(Cosmos::Crc32::DEFAULT_POLY, Cosmos::Crc32::DEFAULT_SEED, true, false)
  File.open("config/data/crc.txt",'w') do |file|
    file.puts "USER_MODIFIED" unless official
    Dir[File.join('lib','**','*')].each do |filename|
      next if File.directory?(filename)
      next if ignore.include?(filename)
      file_data = File.open(filename, 'rb').read.gsub("\x0D\x0A", "\x0A")
      file.puts "\"#{filename}\" #{sprintf("0x%08X", crc.calc(file_data))}"
      count += 1
    end
    Dir[File.join('config','**','*')].each do |filename|
      next if File.directory?(filename)
      next if ignore.include?(filename)
      next if File.basename(filename) == 'crc.txt'
      file_data = File.open(filename, 'rb').read.gsub("\x0D\x0A", "\x0A")
      file.puts "\"#{filename}\" #{sprintf("0x%08X", crc.calc(file_data))}"
      count += 1
    end
    Dir[File.join('tools','**','*')].each do |filename|
      next if File.directory?(filename)
      next if ignore.include?(filename)
      file_data = File.open(filename, 'rb').read.gsub("\x0D\x0A", "\x0A")
      file.puts "\"#{filename}\" #{sprintf("0x%08X", crc.calc(file_data))}"
      count += 1
    end
    Dir[File.join('procedures','**','*')].each do |filename|
      next if File.directory?(filename)
      next if ignore.include?(filename)
      file_data = File.open(filename, 'rb').read.gsub("\x0D\x0A", "\x0A")
      file.puts "\"#{filename}\" #{sprintf("0x%08X", crc.calc(file_data))}"
      count += 1
    end
  end
  puts "Created config/data/crc.txt with #{count} CRCs"
end

task :crc do
  create_crc_file(false)
end

task :crc_official do
  create_crc_file(true)
end
