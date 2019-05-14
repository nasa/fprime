# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'cosmos/packets/packet_config'
require 'cosmos/ext/telemetry' if RUBY_ENGINE == 'ruby' and !ENV['COSMOS_NO_EXT']

module Cosmos

  # Telemetry uses PacketConfig to parse the command and telemetry
  # configuration files. It contains all the knowledge of which telemetry packets
  # exist in the system and how to access them. This class is the API layer
  # which other classes use to access telemetry.
  #
  # This should not be confused with the Api module which implements the JSON
  # API that is used by tools when accessing the Server. The Api module always
  # provides Ruby primatives where the Telemetry class can return actual
  # Packet or PacketItem objects. While there are some overlapping methods between
  # the two, these are separate interfaces into the system.
  class Telemetry
    attr_accessor :config

    LATEST_PACKET_NAME = 'LATEST'.freeze

    # @param config [PacketConfig] Packet configuration to use to access the
    #   telemetry
    def initialize(config)
      @config = config
      @id_value_hash = {}
    end

    # (see PacketConfig#warnings)
    def warnings
      return @config.warnings
    end

    # @return [Array<String>] The telemetry target names (excluding UNKNOWN)
    def target_names
      result = @config.telemetry.keys.sort
      result.delete('UNKNOWN'.freeze)
      return result
    end

    if RUBY_ENGINE != 'ruby' or ENV['COSMOS_NO_EXT']
      # @param target_name [String] The target name
      # @return [Hash<packet_name=>Packet>] Hash of the telemetry packets for the given
      #   target name keyed by the packet name
      def packets(target_name)
        upcase_target_name = target_name.to_s.upcase
        target_packets = @config.telemetry[upcase_target_name]
        raise "Telemetry target '#{upcase_target_name}' does not exist" unless target_packets
        target_packets
      end

      # @param target_name [String] The target name
      # @param packet_name [String] The packet name. Must be a defined packet name
      #   and not 'LATEST'.
      # @return [Packet] The telemetry packet for the given target and packet name
      def packet(target_name, packet_name)
        target_packets = packets(target_name)
        upcase_packet_name = packet_name.to_s.upcase
        packet = target_packets[upcase_packet_name]
        unless packet
          upcase_target_name = target_name.to_s.upcase
          raise "Telemetry packet '#{upcase_target_name} #{upcase_packet_name}' does not exist"
        end
        packet
      end

      # @param target_name (see #packet)
      # @param packet_name [String] The packet name. 'LATEST' can also be given
      #   to specify the last received (or defined if no packets have been
      #   received) packet within the given target that contains the
      #   item_name.
      # @param item_name [String] The item name
      # @return [Packet, PacketItem] The packet and the packet item
      def packet_and_item(target_name, packet_name, item_name)
        upcase_packet_name = packet_name.to_s.upcase
        if upcase_packet_name == "LATEST".freeze
          return_packet = newest_packet(target_name, item_name)
        else
          return_packet = packet(target_name, packet_name)
        end
        item = return_packet.get_item(item_name)
        return [return_packet, item]
      end

      # Return a telemetry value from a packet.
      #
      # @param target_name (see #packet_and_item)
      # @param packet_name (see #packet_and_item)
      # @param item_name (see #packet_and_item)
      # @param value_type [Symbol] How to convert the item before returning.
      #   Must be one of {Packet::VALUE_TYPES}
      # @return The value. :FORMATTED and :WITH_UNITS values are always returned
      #   as Strings. :RAW values will match their data_type. :CONVERTED values
      #   can be any type.
      def value(target_name, packet_name, item_name, value_type = :CONVERTED)
        packet, _ = packet_and_item(target_name, packet_name, item_name) # Handles LATEST
        return packet.read(item_name, value_type)
      end

      # Reads the specified list of items and returns their values and limits
      # state.
      #
      # @param item_array [Array<Array(String String String)>] An array
      #   consisting of [target name, packet name, item name]
      # @param value_types [Symbol|Array<Symbol>] How to convert the items before
      #   returning. A single symbol of {Packet::VALUE_TYPES}
      #   can be passed which will convert all items the same way. Or
      #   an array of symbols can be passed to control how each item is
      #   converted.
      # @return [Array, Array, Array] The first array contains the item values and the
      #   second their limits state, and the third their limits settings which includes
      #   the red, yellow, and green (if given) limits values.
      def values_and_limits_states(item_array, value_types = :CONVERTED)
        items = []

        # Verify item_array is a nested array
        raise(ArgumentError, "item_array must be a nested array consisting of [[tgt,pkt,item],[tgt,pkt,item],...]") unless Array === item_array[0]

        states = []
        settings = []
        limits_set = System.limits_set

        raise(ArgumentError, "Passed #{item_array.length} items but only #{value_types.length} value types") if (Array === value_types) and item_array.length != value_types.length

        value_type = value_types.intern unless Array === value_types
        item_array.length.times do |index|
          entry = item_array[index]
          target_name = entry[0]
          packet_name = entry[1]
          item_name = entry[2]
          value_type = value_types[index].intern if Array === value_types

          packet, item = packet_and_item(target_name, packet_name, item_name) # Handles LATEST
          items << packet.read(item_name, value_type)
          limits = item.limits
          states << limits.state
          limits_values = limits.values
          if limits_values
            limits_settings = limits_values[limits_set]
          else
            limits_settings = nil
          end
          settings << limits_settings
        end

        return [items, states, settings]
      end
    end

    # @param target_name (see #packet)
    # @param packet_name (see #packet)
    # @return [Array<PacketItem>] The telemetry items for the given target and packet name
    def items(target_name, packet_name)
      return packet(target_name, packet_name).sorted_items
    end

    # @param target_name (see #packet)
    # @param packet_name (see #packet) The packet name.  LATEST is supported.
    # @return [Array<PacketItem>] The telemetry item names for the given target and packet name
    def item_names(target_name, packet_name)
      if LATEST_PACKET_NAME.casecmp(packet_name).zero?
        target_upcase = target_name.to_s.upcase
        target_latest_data = @config.latest_data[target_upcase]
        raise "Telemetry Target '#{target_upcase}' does not exist" unless target_latest_data
        item_names = target_latest_data.keys
      else
        tlm_packet = packet(target_name, packet_name)
        item_names = []
        tlm_packet.sorted_items.each {|item| item_names << item.name}
      end
      item_names
    end

    # Set a telemetry value in a packet.
    #
    # @param target_name (see #packet_and_item)
    # @param packet_name (see #packet_and_item)
    # @param item_name (see #packet_and_item)
    # @param value The value to set in the packet
    # @param value_type (see #tlm)
    def set_value(target_name, packet_name, item_name, value, value_type = :CONVERTED)
      packet, _ = packet_and_item(target_name, packet_name, item_name)
      packet.write(item_name, value, value_type)
    end

    # @param target_name (see #packet_and_item)
    # @param item_name (see #packet_and_item)
    # @return [Array<Packet>] The latest (most recently arrived) packets with
    #   the specified target and item.
    def latest_packets(target_name, item_name)
      target_upcase = target_name.to_s.upcase
      item_upcase = item_name.to_s.upcase
      target_latest_data = @config.latest_data[target_upcase]
      raise "Telemetry target '#{target_upcase}' does not exist" unless target_latest_data
      packets = @config.latest_data[target_upcase][item_upcase]
      raise "Telemetry item '#{target_upcase} #{LATEST_PACKET_NAME} #{item_upcase}' does not exist" unless packets
      return packets
    end

    # @param target_name (see #packet_and_item)
    # @param item_name (see #packet_and_item)
    # @return [Packet] The packet with the most recent timestamp that contains
    #   the specified target and item.
    def newest_packet(target_name, item_name)
      # Handle LATEST_PACKET_NAME - Lookup packets for this target/item
      packets = latest_packets(target_name, item_name)

      # Find packet with newest timestamp
      newest_packet = nil
      newest_received_time = nil
      packets.each do |packet|
        received_time = packet.received_time
        if newest_received_time
          # See if the received time from this packet is newer.
          # Having the >= makes this method return the last defined packet
          # whether the timestamps are both nil or both equal.
          if received_time and received_time >= newest_received_time
            newest_packet = packet
            newest_received_time = newest_packet.received_time
          end
        else
          # No received time yet so take this packet
          newest_packet = packet
          newest_received_time = newest_packet.received_time
        end
      end
      return newest_packet
    end

    # Identifies an unknown buffer of data as a defined packet and sets the
    # packet's data to the given buffer. Identifying a packet uses the fields
    # marked as ID_ITEM to identify if the buffer passed represents the
    # packet defined. Incorrectly sized buffers are still processed but an
    # error is logged.
    #
    # Note: This affects all subsequent requests for the packet (for example
    # using packet) which is why the method is marked with a bang!
    #
    # @param packet_data [String] The binary packet data buffer
    # @param target_names [Array<String>] List of target names to limit the search. The
    #   default value of nil means to search all known targets.
    # @return [Packet] The identified packet with its data set to the given
    #   packet_data buffer. Returns nil if no packet could be identified.
    def identify!(packet_data, target_names = nil, fast_identify = true)
      identified_packet = identify(packet_data, target_names, fast_identify)
      identified_packet.buffer = packet_data if identified_packet
      return identified_packet
    end

    # Finds a packet from the Current Value Table that matches the given data
    # and returns it.  Does not fill the packets buffer.  Use identify! to update the CVT.
    #
    # @param packet_data [String] The binary packet data buffer
    # @param target_names [Array<String>] List of target names to limit the search. The
    #   default value of nil means to search all known targets.
    # @return [Packet] The identified packet, Returns nil if no packet could be identified.
    def identify(packet_data, target_names = nil, fast_identify = true)
      target_names = target_names() unless target_names

      target_names.each do |target_name|
        target_packets = nil
        begin
          target_packets = packets(target_name)
          #puts target_packets.length
        rescue RuntimeError
          # No telemetry for this target
          next
        end

        # Iterate through the packets and see if any represent the buffer
        if fast_identify
          packet = target_packets.first[1]
          key = packet.read_id_values(packet_data)
          hash = target_id_value_hash(target_name)
          result = hash[key]
          result = hash['CATCHALL'.freeze] unless result
          return result
        else
          target_packets.each do |packet_name, packet|
            return packet if packet.identify?(packet_data)
          end
        end
      end

      return nil
    end

    def identify_and_define_packet(packet, target_names = nil, fast_identify = true)
      if !packet.identified?
        identified_packet = identify(packet.buffer(false), target_names, fast_identify)
        return nil unless identified_packet
        identified_packet = identified_packet.clone
        identified_packet.buffer = packet.buffer
        identified_packet.received_time = packet.received_time
        identified_packet.stored = packet.stored
        identified_packet.extra = packet.extra
        return identified_packet
      end

      if !packet.defined?
        begin
          identified_packet = self.packet(packet.target_name, packet.packet_name)
        rescue RuntimeError
          return nil
        end
        identified_packet = identified_packet.clone
        identified_packet.buffer = packet.buffer
        identified_packet.received_time = packet.received_time
        identified_packet.stored = packet.stored
        identified_packet.extra = packet.extra
        return identified_packet
      end

      return packet
    end

    def target_id_value_hash(target_name)
      target_name_upcase = target_name.to_s.upcase
      hash = @id_value_hash[target_name_upcase]
      return hash if hash
      
      hash = {}
      @id_value_hash[target_name_upcase] = hash
      
      target_packets = packets(target_name_upcase)
      target_packets.each do |packet_name, packet|
        if packet.id_items.length > 0
          key = []
          packet.id_items.each do |item|
            key << item.id_value
          end
          hash[key] = packet
        else
          hash['CATCHALL'.freeze] = packet
        end
      end
      
      hash
    end

    # Updates the specified packet with the given packet data. Raises an error
    # if the packet could not be found.
    #
    # Note: This affects all subsequent requests for the packet which is why
    # the method is marked with a bang!
    #
    # @param target_name (see #packet)
    # @param packet_name (see #packet)
    # @param packet_data (see #identify_tlm!)
    # @return [Packet] The packet with its data set to the given packet_data
    #   buffer.
    def update!(target_name, packet_name, packet_data)
      identified_packet = packet(target_name, packet_name)
      identified_packet.buffer = packet_data
      return identified_packet
    end

    # Assigns a limits change callback to all telemetry packets
    #
    # @param limits_change_callback
    def limits_change_callback=(limits_change_callback)
      @config.telemetry.each do |target_name, packets|
        packets.each do |packet_name, packet|
          packet.limits_change_callback = limits_change_callback
        end
      end
    end

    # Iterates through all the telemetry packets and marks them stale if they
    # haven't been received for over the System.staleness_seconds value.
    #
    # @return [Array(Packet)] Array of the stale packets
    def check_stale
      stale = []
      time = Time.now.sys
      @config.telemetry.each do |target_name, target_packets|
        target_packets.each do |packet_name, packet|
          if packet.received_time and (!packet.stale) and (time - packet.received_time > System.staleness_seconds)
            packet.set_stale
            stale << packet
          end
        end
      end
      stale
    end

    # @param with_limits_only [Boolean] Return only the stale packets
    #   that have limits items and thus affect the overall limits
    #   state of the system
    # @param target [String] Target name or nil for all targets
    # @return [Array(Packet)] Array of the stale packets
    def stale(with_limits_only = false, target = nil)
      if target && !target_names.include?(target)
        raise "Telemetry target '#{target.upcase}' does not exist"
      end
      stale = []
      @config.telemetry.each do |target_name, target_packets|
        next if (target && target != target_name)
        next if target_name == 'UNKNOWN'
        target_packets.each do |packet_name, packet|
          if packet.stale
            next if (with_limits_only && packet.limits_items.empty?)
            stale << packet
          end
        end
      end
      stale
    end

    # Clears the received_count value on every packet in every target
    def clear_counters
      @config.telemetry.each do |target_name, target_packets|
        target_packets.each do |packet_name, packet|
          packet.received_count = 0
        end
      end
    end

    # Resets metadata on every packet in every target
    def reset
      @config.telemetry.each do |target_name, target_packets|
        target_packets.each do |packet_name, packet|
          packet.reset
        end
      end
    end

    # Returns the first non-hidden packet
    def first_non_hidden
      @config.telemetry.each do |target_name, target_packets|
        next if target_name == 'UNKNOWN'
        target_packets.each do |packet_name, packet|
          return packet unless packet.hidden
        end
      end
      nil
    end

    # Returns an array with a "TARGET_NAME PACKET_NAME ITEM_NAME" string for every item in the system
    def all_item_strings(include_hidden = false, splash = nil)
      strings = []
      tnames = target_names()
      total = tnames.length.to_f
      tnames.each_with_index do |target_name, index|
        if splash
          splash.message = "Processing #{target_name} telemetry"
          splash.progress = index / total
        end

        ignored_items = System.targets[target_name].ignored_items

        packets(target_name).each do |packet_name, packet|
          # We don't audit against hidden or disabled packets
          next if !include_hidden and (packet.hidden || packet.disabled)
          packet.items.each_key do |item_name|
            # Skip ignored items
            next if !include_hidden and ignored_items.include? item_name
            strings << "#{target_name} #{packet_name} #{item_name}"
          end
        end
      end
      strings
    end

    # @return [Hash{String=>Hash{String=>Packet}}] Hash of all the telemetry
    #   packets keyed by the target name. The value is another hash keyed by the
    #   packet name returning the packet.
    def all
      @config.telemetry
    end

  end # class Telemetry

end # module Cosmos
