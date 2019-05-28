# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'cosmos'
require 'cosmos/interfaces'
require 'cosmos/tools/cmd_tlm_server/interface_thread'

module Cosmos
  class ExampleTarget
    class ExampleServerInterface < TcpipServerInterface
      PORT = 9999

      def initialize
        super(PORT, PORT, 5.0, nil, 'LENGTH', 0, 32, 4, 1, 'BIG_ENDIAN', 4, nil, nil, true)
      end
    end

    class ExampleInterfaceThread < InterfaceThread
      protected
      def handle_packet(packet)
        identified_packet = System.commands.identify(packet.buffer, ['EXAMPLE'])
        if identified_packet
          Logger.info "Received command: #{identified_packet.target_name} #{identified_packet.packet_name}"
        else
          Logger.info "Received UNKNOWN command"
        end
      end
    end

    class ExampleTelemetryThread
      attr_reader :thread

      def initialize(interface)
        @interface = interface
        @sleeper = Sleeper.new
      end

      def start
        packet = System.telemetry.packet('EXAMPLE', 'STATUS')
        @thread = Thread.new do
          @stop_thread = false
          begin
            loop do
              packet.write('PACKET_ID', 1)
              packet.write('STRING', "The time is now: #{Time.now.sys.formatted}")
              @interface.write(packet)
              break if @sleeper.sleep(1)
            end
          rescue Exception => err
            Logger.error "ExampleTelemetryThread unexpectedly died\n#{err.formatted}"
          end
        end
      end

      def stop
        Cosmos.kill_thread(self, @thread)
      end

      def graceful_kill
        @sleeper.cancel
      end
    end

    def initialize
      # Create interface to receive commands and send telemetry
      @interface = ExampleServerInterface.new
      @interface_thread = nil
      @telemetry_thread = nil
    end

    def start
      @interface_thread = ExampleInterfaceThread.new(@interface)
      @interface_thread.start
      @telemetry_thread = ExampleTelemetryThread.new(@interface)
      @telemetry_thread.start
    end

    def stop
      @telemetry_thread.stop if @telemetry_thread
      @interface_thread.stop if @interface_thread
    end

    def self.run
      Logger.level = Logger::INFO
      target = self.new
      begin
        target.start
        while true
          sleep 1
        end
      rescue SystemExit, Interrupt
        target.stop
      end
    end
  end
end
