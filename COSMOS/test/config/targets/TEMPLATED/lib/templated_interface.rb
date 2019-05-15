# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'cosmos'

module Cosmos

  class TemplatedInterface < TcpipClientInterface
    def initialize(*args)
      super(*args)
      @polling_thread = nil
      @sleeper = Sleeper.new
    end

    def connect
      super()

      # Start a thread to poll telemetry
      Thread.new do |thread|
        Cosmos.kill_thread(self, @polling_thread)
        @sleeper = Sleeper.new
        @polling_thread = Thread.current
        begin
          # Wait for the connection to actually occur
          while !connected?
            @sleeper.sleep(1)
          end
          loop do
            cmd("#{@target_names[0]} GET_SETPT_VOLTAGE")
            break if @sleeper.sleep(1)
          end
        rescue Errno::ECONNRESET
          # This typically means the target disconnected
        rescue Exception => err
          Logger.error "Polling Thread Unexpectedly Died.\n#{err.formatted}"
        end
      end
    end

    def disconnect
      super()
      # Note: This must be after super or the disconnect process
      # will be interrupted by killing the thread
      Cosmos.kill_thread(self, @polling_thread) if Thread.current != @polling_thread
      @polling_thread = nil
    end

    def graceful_kill
      @sleeper.cancel
    end
  end
end
