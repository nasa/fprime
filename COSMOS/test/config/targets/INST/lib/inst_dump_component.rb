# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'cosmos'
require 'cosmos/tools/data_viewer/data_viewer_component'

module Cosmos
  # This class displays packets as raw hex values
  class InstDumpComponent < DataViewerComponent

    # Processes the given packet. No gui interaction should be done in this
    # method. Override this method for other components.
    def process_packet (packet)
      processed_text = ''
      processed_text << '*' * 80 << "\n"
      processed_text << "* #{packet.target_name} #{packet.packet_name}\n"
      processed_text << "* Received Time: #{packet.received_time.formatted}\n" if packet.received_time
      processed_text << "* Received Count: #{packet.received_count}\n"
      processed_text << '*' * 80 << "\n"
      processed_text << packet.buffer.formatted << "\n"
      # Ensure that queue does not grow infinitely while paused
      if @processed_queue.length < 1000
        @processed_queue << processed_text
      end
    end
  end
end
