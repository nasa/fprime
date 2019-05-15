# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

# Provides a demonstration of a Simulated Target

require 'cosmos'

module Cosmos

  class SimInst < SimulatedTarget

    SOLAR_PANEL_DFLTS = [-179.0, 179.0, -179.0, 179.0, -95.0] unless defined? SOLAR_PANEL_DFLTS

    def initialize(target_name)
      super(target_name)

      position_filename = File.join(::Cosmos::USERPATH, 'config', 'data', 'position.bin')
      attitude_filename = File.join(::Cosmos::USERPATH, 'config', 'data', 'attitude.bin')
      @position_file = File.open(position_filename, 'rb')
      @attitude_file = File.open(attitude_filename, 'rb')
      @position_file_size = File.size(position_filename)
      @attitude_file_size = File.size(attitude_filename)
      @position_file_bytes_read = 0
      @attitude_file_bytes_read = 0

      @pos_packet = Structure.new(:BIG_ENDIAN)
      @pos_packet.append_item('DAY', 16, :UINT)
      @pos_packet.append_item('MSOD', 32, :UINT)
      @pos_packet.append_item('USOMS', 16, :UINT)
      @pos_packet.append_item('POSX', 32, :FLOAT)
      @pos_packet.append_item('POSY', 32, :FLOAT)
      @pos_packet.append_item('POSZ', 32, :FLOAT)
      @pos_packet.append_item('SPARE1', 16, :UINT)
      @pos_packet.append_item('SPARE2', 32, :UINT)
      @pos_packet.append_item('SPARE3', 16, :UINT)
      @pos_packet.append_item('VELX', 32, :FLOAT)
      @pos_packet.append_item('VELY', 32, :FLOAT)
      @pos_packet.append_item('VELZ', 32, :FLOAT)
      @pos_packet.append_item('SPARE4', 32, :UINT)
      @pos_packet.enable_method_missing

      @att_packet = Structure.new(:BIG_ENDIAN)
      @att_packet.append_item('DAY', 16, :UINT)
      @att_packet.append_item('MSOD', 32, :UINT)
      @att_packet.append_item('USOMS', 16, :UINT)
      @att_packet.append_item('Q1', 32, :FLOAT)
      @att_packet.append_item('Q2', 32, :FLOAT)
      @att_packet.append_item('Q3', 32, :FLOAT)
      @att_packet.append_item('Q4', 32, :FLOAT)
      @att_packet.append_item('BIASX', 32, :FLOAT)
      @att_packet.append_item('BIASY', 32, :FLOAT)
      @att_packet.append_item('BIASZ', 32, :FLOAT)
      @att_packet.append_item('SPARE', 32, :FLOAT)
      @att_packet.enable_method_missing

      packet = @tlm_packets['HEALTH_STATUS']
      packet.enable_method_missing
      packet.CcsdsSeqFlags = 'NOGROUP'
      packet.CcsdsLength = packet.buffer.length - 7
      packet.temp1 = 50.0
      packet.temp2 = -20.0
      packet.temp3 = 85.0
      packet.temp4 = 0.0
      packet.duration = 10.0
      packet.collect_type = 'NORMAL'

      packet = @tlm_packets['ADCS']
      packet.enable_method_missing
      packet.CcsdsSeqFlags = 'NOGROUP'
      packet.CcsdsLength   = packet.buffer.length - 7

      packet = @tlm_packets['PARAMS']
      packet.enable_method_missing
      packet.CcsdsSeqFlags = 'NOGROUP'
      packet.CcsdsLength = packet.buffer.length - 7

      packet = @tlm_packets['IMAGE']
      packet.enable_method_missing
      packet.CcsdsSeqFlags = 'NOGROUP'
      packet.CcsdsLength = packet.buffer.length - 7

      packet = @tlm_packets['MECH']
      packet.enable_method_missing
      packet.CcsdsSeqFlags = 'NOGROUP'
      packet.CcsdsLength = packet.buffer.length - 7

      @solar_panel_positions = SOLAR_PANEL_DFLTS.dup
      @solar_panel_thread = nil
      @solar_panel_thread_cancel = false

      @trackStars = Array.new
      @trackStars[0] = 1237
      @trackStars[1] = 1329
      @trackStars[2] = 1333
      @trackStars[3] = 1139
      @trackStars[4] = 1161
      @trackStars[5] = 682
      @trackStars[6] = 717
      @trackStars[7] = 814
      @trackStars[8] = 583
      @trackStars[9] = 622

      @get_count = 0
      @bad_temp2 = false
      @last_temp2 = 0
    end

    def set_rates
      set_rate('ADCS', 10)
      set_rate('HEALTH_STATUS', 100)
      set_rate('PARAMS', 100)
      set_rate('IMAGE', 100)
      set_rate('MECH', 10)
    end

    def write(packet)
      name = packet.packet_name.upcase

      hs_packet = @tlm_packets['HEALTH_STATUS']
      params_packet = @tlm_packets['PARAMS']

      case name
      when 'COLLECT'
        hs_packet.collects += 1
        hs_packet.duration = packet.read('duration')
        hs_packet.collect_type = packet.read("type")
      when 'CLEAR'
        hs_packet.collects = 0
      when 'SETPARAMS'
        params_packet.value1 = packet.read('value1')
        params_packet.value2 = packet.read('value2')
        params_packet.value3 = packet.read('value3')
        params_packet.value4 = packet.read('value4')
        params_packet.value5 = packet.read('value5')
      when 'ASCIICMD'
        hs_packet.asciicmd = packet.read('string')
      when 'SLRPNLDEPLOY'
        return if @solar_panel_thread and @solar_panel_thread.alive?
        @solar_panel_thread = Thread.new do
          @solar_panel_thread_cancel = false
          (0..@solar_panel_positions.size-1).to_a.reverse.each do |i|
            while (@solar_panel_positions[i] > 0.1) or (@solar_panel_positions[i] < - 0.1)
              if @solar_panel_positions[i] > 3.0
                @solar_panel_positions[i] -= 3.0
              elsif @solar_panel_positions[i] < -3.0
                @solar_panel_positions[i] += 3.0
              else
                @solar_panel_positions[i] = 0.0
              end
              sleep(0.10)
              break if @solar_panel_thread_cancel
            end
            if @solar_panel_thread_cancel
              @solar_panel_thread_cancel = false
              break
            end
          end
        end
      when 'SLRPNLRESET'
        Cosmos.kill_thread(self, @solar_panel_thread)
        @solar_panel_positions = SOLAR_PANEL_DFLTS.dup
      end
    end

    def graceful_kill
      @solar_panel_thread_cancel = true
    end

    def read(count_100hz, time)
      pending_packets = get_pending_packets(count_100hz)

      pending_packets.each do |packet|
        case packet.packet_name
        when 'ADCS'
          #Read 44 Bytes for Position Data
          pos_data = nil
          begin
            pos_data = @position_file.read(44)
            @position_file_bytes_read += 44
          rescue
            # Do Nothing
          end

          if pos_data.nil? or pos_data.length == 0
            #Assume end of file - close and reopen
            @position_file.close
            @position_file = File.open(File.join(::Cosmos::USERPATH, 'config', 'data', 'position.bin'), 'rb')
            pos_data = @position_file.read(44)
            @position_file_bytes_read = 44
          end

          @pos_packet.buffer = pos_data
          packet.posx = @pos_packet.posx
          packet.posy = @pos_packet.posy
          packet.posz = @pos_packet.posz
          packet.velx = @pos_packet.velx
          packet.vely = @pos_packet.vely
          packet.velz = @pos_packet.velz

          #Read 40 Bytes for Attitude Data
          att_data = nil
          begin
            att_data = @attitude_file.read(40)
            @attitude_file_bytes_read += 40
          rescue
            # Do Nothing
          end

          if att_data.nil? or att_data.length == 0
            @attitude_file.close
            @attitude_file = File.open(File.join(::Cosmos::USERPATH, 'config', 'data', 'attitude.bin'), 'rb')
            att_data = @attitude_file.read(40)
            @attitude_file_bytes_read = 40
          end

          @att_packet.buffer = att_data
          packet.q1 = @att_packet.q1
          packet.q2 = @att_packet.q2
          packet.q3 = @att_packet.q3
          packet.q4 = @att_packet.q4
          packet.biasx = @att_packet.biasx
          packet.biasy = @att_packet.biasy
          packet.biasy = @att_packet.biasz

          packet.star1id = @trackStars[((@get_count / 100) + 0) % 10]
          packet.star2id = @trackStars[((@get_count / 100) + 1) % 10]
          packet.star3id = @trackStars[((@get_count / 100) + 2) % 10]
          packet.star4id = @trackStars[((@get_count / 100) + 3) % 10]
          packet.star5id = @trackStars[((@get_count / 100) + 4) % 10]

          packet.posprogress = (@position_file_bytes_read.to_f / @position_file_size.to_f) * 100.0
          packet.attprogress = (@attitude_file_bytes_read.to_f / @attitude_file_size.to_f) * 100.0

          packet.timesec = time.tv_sec
          packet.timeus  = time.tv_usec
          packet.ccsdsseqcnt += 1

        when 'HEALTH_STATUS'
          cycle_tlm_item(packet, 'temp1', -95.0, 95.0, 5.0)
          if @bad_temp2
            packet.write('temp2', @last_temp2)
            @bad_temp2 = false
          end
          @last_temp2 = cycle_tlm_item(packet, 'temp2', -50.0, 50.0, -1.0)
          if (packet.temp2.abs - 30).abs < 2
            packet.write('temp2', Float::NAN)
            @bad_temp2 = true
          elsif (packet.temp2.abs - 20).abs < 2
            packet.write('temp2', -Float::INFINITY)
            @bad_temp2 = true
          elsif (packet.temp2.abs - 10).abs < 2
            packet.write('temp2', Float::INFINITY)
            @bad_temp2 = true
          end
          cycle_tlm_item(packet, 'temp3', -30.0, 80.0, 2.0)
          cycle_tlm_item(packet, 'temp4', 0.0, 20.0, -0.1)

          packet.timesec = time.tv_sec
          packet.timeus  = time.tv_usec
          packet.ccsdsseqcnt += 1

          ary = []
          10.times do |index|
            ary << index
          end
          packet.ary = ary

          if @get_count % 1000 == 0
            if packet.ground1status == 'CONNECTED'
              packet.ground1status = 'UNAVAILABLE'
            else
              packet.ground1status = 'CONNECTED'
            end
          end

          if @get_count % 500 == 0
            if packet.ground2status == 'CONNECTED'
              packet.ground2status = 'UNAVAILABLE'
            else
              packet.ground2status = 'CONNECTED'
            end
          end

        when 'PARAMS'
          packet.timesec = time.tv_sec
          packet.timeus = time.tv_usec
          packet.ccsdsseqcnt += 1

        when 'IMAGE'
          packet.timesec = time.tv_sec
          packet.timeus = time.tv_usec
          packet.ccsdsseqcnt += 1

        when 'MECH'
          packet.timesec = time.tv_sec
          packet.timeus = time.tv_usec
          packet.ccsdsseqcnt += 1
          packet.slrpnl1 = @solar_panel_positions[0]
          packet.slrpnl2 = @solar_panel_positions[1]
          packet.slrpnl3 = @solar_panel_positions[2]
          packet.slrpnl4 = @solar_panel_positions[3]
          packet.slrpnl5 = @solar_panel_positions[4]
        end
      end

      pending_packets << Packet.new(nil, nil, :BIG_ENDIAN, nil, "\000" * 10) if @get_count == 300

      @get_count += 1
      pending_packets
    end

  end # class SimInst

end # module Cosmos
