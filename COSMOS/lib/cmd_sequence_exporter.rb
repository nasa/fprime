# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'cosmos'
require 'cosmos/ccsds/ccsds_packet'

module Cosmos
  class CmdSequenceExporter
    def initialize(parent)
      @parent = parent
    end

    def export(filename, sequence_dir, sequence_list)
      basename = File.basename(filename, ".*")
      filename = Qt::FileDialog::getSaveFileName(@parent,   # parent
                                                 'Export', # caption
                                                 sequence_dir + "/#{basename}.bin", # dir
                                                 'Sequence Binary (*.bin)') # filter
      return if filename.nil? || filename.empty?

      data = ''
      sequence_list.each do |item|
        begin
          time = Time.parse(item.time)
          day, ms, us = Time.mdy2ccsds(time.year, time.month, time.day, time.hour, time.min, time.sec, time.usec)
          data << [day].pack("N") # UINT32
          data << [ms].pack("N") # UINT32
        rescue
          time = item.time.to_f * 1000
          data << [0].pack("N") # UINT32
          data << [time].pack("N") # UINT32
        end
        data << item.command.buffer
      end
      ccsds = CcsdsPacket.new
      ccsds.write("CCSDSTYPE", CcsdsPacket::COMMAND)
      ccsds.write('CCSDSSHF', 0)
      ccsds.write("CCSDSAPID", 505)
      ccsds.write("CCSDSSEQFLAGS", CcsdsPacket::STANDALONE)
      ccsds.write("CCSDSDATA", data)
      ccsds.write("CCSDSLENGTH", data.length - 1)
      File.open(filename, 'wb') { |file| file.write ccsds.buffer }
    end
  end
end
