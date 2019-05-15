#!/usr/bin/env ruby
# encoding: ascii-8bit

# Copyright 2015 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

def tool_launch
  begin
    require 'bundler/setup'
    require 'cosmos'
    yield
  rescue Exception => error
    popup_error = error; popup_error = $cosmos_fatal_exception if defined? $cosmos_fatal_exception
    begin
      raise error if STDIN.isatty # Have a console
      raise error unless defined? $cosmos_fatal_exception or (error.class != SystemExit and error.class != Interrupt)
      case RUBY_PLATFORM
      when /mingw32/
        require 'fiddle'
        Fiddle::Function.new(Fiddle.dlopen('user32')['MessageBox'], [Fiddle::TYPE_LONG, Fiddle::TYPE_VOIDP, Fiddle::TYPE_VOIDP, Fiddle::TYPE_LONG], Fiddle::TYPE_LONG).call(0, "#{popup_error.class}:#{popup_error.message}\n\n#{popup_error.backtrace.join("\n")}\n\nNote: Ctrl-C will copy this information to the clipboard.", "Error Starting COSMOS Tool", 0x50030)
      when /darwin/
        system("osascript -e 'display notification \"#{popup_error.class}:#{popup_error.message}:#{popup_error.backtrace[0].tr("'\"`<>", '')}\" with title \"Error Starting COSMOS Tool\"'")
      else
        message = "#{popup_error.class}:#{popup_error.message}\\n\\n#{popup_error.backtrace.join("\\n").tr("'\"`<>", '')}"
        command = "zenity --info --text=\"#{message}\" --title=\"Error Starting COSMOS Tool\""
        success = system(command)
        system("notify-send \"Error Starting COSMOS Tool\" \"#{message}\"") unless success
      end
    ensure
      raise error
    end
  end
end
