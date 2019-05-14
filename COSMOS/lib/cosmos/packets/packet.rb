# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'digest/md5'
require 'cosmos/packets/structure'
require 'cosmos/packets/packet_item'
require 'cosmos/ext/packet' if RUBY_ENGINE == 'ruby' and !ENV['COSMOS_NO_EXT']

module Cosmos
  # Adds features common to all COSMOS packets of data to the Structure class.
  # This includes the additional attributes listed below. The primary behavior
  # Packet adds is the ability to apply formatting to PacketItem values as well
  # as managing PacketItem's limit states.
  class Packet < Structure
    RESERVED_ITEM_NAMES = ['PACKET_TIMESECONDS'.freeze, 'PACKET_TIMEFORMATTED'.freeze, 'RECEIVED_TIMESECONDS'.freeze, 'RECEIVED_TIMEFORMATTED'.freeze, 'RECEIVED_COUNT'.freeze]
    CATCH_ALL_STATE = 'ANY'

    # @return [String] Name of the target this packet is associated with
    attr_reader :target_name

    # @return [String] Name of the packet
    attr_reader :packet_name

    # @return [String] Description of the packet
    attr_reader :description

    # @return [Time] Time at which the packet was received
    attr_reader :received_time

    # @return [Integer] Number of times the packet has been received
    attr_reader :received_count

    # @return [Boolean] Flag indicating if the packet is hazardous (typically for commands)
    attr_accessor :hazardous

    # @return [String] Description of why the packet is hazardous
    attr_reader :hazardous_description

    # Contains the values given by the user for a command (distinguished from defaults)
    # These values should be used within command conversions if present because the order
    # that values are written into the actual packet can vary
    # @return [Hash<Item Name, Value>] Given values when constructing the packet
    attr_reader :given_values

    # @return [Boolean] Flag indicating if the packet is stale (hasn't been received recently)
    attr_reader :stale

    # @return [Boolean] Whether or not this is a 'raw' packet
    attr_accessor :raw

    # @return [Boolean] Whether or not this is a 'hidden' packet
    attr_accessor :hidden

    # @return [Boolean] Whether or not this is a 'disabled' packet
    attr_accessor :disabled

    # @return [Boolean] Whether or not messages should be printed for this packet
    attr_accessor :messages_disabled

    # @return [Boolean] Whether or not this is a 'abstract' packet
    attr_accessor :abstract

    # @return [Boolean] Whether or not this was a stored packet
    attr_accessor :stored

    # @return [Hash] Extra data to be logged/transferred with packet
    attr_accessor :extra

    # Valid format types
    VALUE_TYPES = [:RAW, :CONVERTED, :FORMATTED, :WITH_UNITS]

    if RUBY_ENGINE != 'ruby' or ENV['COSMOS_NO_EXT']
      # Creates a new packet by initalizing the attributes.
      #
      # @param target_name [String] Name of the target this packet is associated with
      # @param packet_name [String] Name of the packet
      # @param default_endianness [Symbol] One of {BinaryAccessor::ENDIANNESS}
      # @param description [String] Description of the packet
      # @param buffer [String] String buffer to hold the packet data
      # @param item_class [Class] Class used to instantiate items (Must be a
      #   subclass of PacketItem)
      def initialize(target_name, packet_name, default_endianness = :BIG_ENDIAN, description = nil, buffer = '', item_class = PacketItem)
        super(default_endianness, buffer, item_class)
        self.target_name = target_name
        self.packet_name = packet_name
        self.description = description
        @received_time = nil
        @received_count = 0
        @id_items = nil
        @hazardous = false
        @hazardous_description = nil
        @given_values = nil
        @limits_items = nil
        @processors = nil
        @stale = true
        @limits_change_callback = nil
        @read_conversion_cache = nil
        @raw = nil
        @messages_disabled = false
        @meta = nil
        @hidden = false
        @disabled = false
        @stored = false
        @extra = nil
      end

      # Sets the target name this packet is associated with. Unidentified packets
      # will have target name set to nil.
      #
      # @param target_name [String] Name of the target this packet is associated with
      def target_name=(target_name)
        if target_name
          if !(String === target_name)
            raise(ArgumentError, "target_name must be a String but is a #{target_name.class}")
          end
          @target_name = target_name.upcase.freeze
        else
          @target_name = nil
        end
        @target_name
      end

      # Sets the packet name. Unidentified packets will have packet name set to
      # nil.
      #
      # @param packet_name [String] Name of the packet
      def packet_name=(packet_name)
        if packet_name
          if !(String === packet_name)
            raise(ArgumentError, "packet_name must be a String but is a #{packet_name.class}")
          end
          @packet_name = packet_name.upcase.freeze
        else
          @packet_name = nil
        end
        @packet_name
      end

      # Sets the description of the packet
      #
      # @param description [String] Description of the packet
      def description=(description)
        if description
          if !(String === description)
            raise(ArgumentError, "description must be a String but is a #{description.class}")
          end
          @description = description.clone.freeze
        else
          @description = nil
        end
        @description
      end

      # Sets the received time of the packet
      #
      # @param received_time [Time] Time this packet was received
      def received_time=(received_time)
        if received_time
          if !(Time === received_time)
            raise(ArgumentError, "received_time must be a Time but is a #{received_time.class}")
          end
          @received_time = received_time.clone.freeze
        else
          @received_time = nil
        end
        @read_conversion_cache.clear if @read_conversion_cache
        @received_time
      end

      # Sets the received count of the packet
      #
      # @param received_count [Integer] Number of times this packet has been
      #   received
      def received_count=(received_count)
        if !(Integer === received_count)
          raise(ArgumentError, "received_count must be an Integer but is a #{received_count.class}")
        end
        @received_count = received_count
        @read_conversion_cache.clear if @read_conversion_cache
        @received_count
      end
      
    end # if RUBY_ENGINE != 'ruby' or ENV['COSMOS_NO_EXT']      
      
    # Tries to identify if a buffer represents the currently defined packet. It
    # does this by iterating over all the packet items that were created with
    # an ID value and checking whether that ID value is present at the correct
    # location in the buffer.
    #
    # Incorrectly sized buffers will still positively identify if there is
    # enough data to match the ID values. This is to allow incorrectly sized
    # packets to still be processed as well as possible given the incorrectly
    # sized data.
    #
    # @param buffer [String] Raw buffer of binary data
    # @return [Boolean] Whether or not the buffer of data is this packet
    def identify?(buffer)
      return false unless buffer
      return true unless @id_items

      @id_items.each do |item|
        begin
          value = read_item(item, :RAW, buffer)
        rescue Exception
          value = nil
        end
        return false if item.id_value != value
      end

      true
    end
    
    def read_id_values(buffer)
      return [] unless buffer
      return [] unless @id_items
      values = []
      
      @id_items.each do |item|
        begin
          values << read_item(item, :RAW, buffer)
        rescue Exception
          values << nil
        end
      end
      
      values
    end
    
    # Returns @received_time unless a packet item called PACKET_TIME exists that returns
    # a Ruby Time object that represents a different timestamp for the packet
    def packet_time
      item = @items['PACKET_TIME'.freeze]
      if item
        return read_item(item, :CONVERTED, @buffer)
      else
        return @received_time
      end
    end

    # Calculates a unique MD5Sum that changes if the parts of the packet configuration change that could affect
    # the "shape" of the packet.  This value is cached and that packet should not be changed if this method is being used
    def config_name
      return @config_name if @config_name
      string = "#{self.target_name} #{self.packet_name}"
      @sorted_items.each do |item|
        string << " ITEM #{item.name} #{item.bit_offset} #{item.bit_size} #{item.data_type} #{item.array_size} #{item.endianness} #{item.overflow} #{item.states} #{item.read_conversion ? item.read_conversion.class : 'NO_CONVERSION'}"
      end
      digest = Digest::MD5.new
      digest << string
      @config_name = digest.hexdigest
      @config_name
    end

    # (see Structure#buffer=)
    def buffer=(buffer)
      synchronize() do
        begin
          internal_buffer_equals(buffer)
        rescue RuntimeError
          Logger.instance.error "#{@target_name} #{@packet_name} received with actual packet length of #{buffer.length} but defined length of #{@defined_length}"
        end
        @read_conversion_cache.clear if @read_conversion_cache
        process()
      end
    end

    # Sets the received time of the packet (without cloning)
    #
    # @param received_time [Time] Time this packet was received
    def set_received_time_fast(received_time)
      @received_time = received_time
      @received_time.freeze if @received_time
      if @read_conversion_cache
        synchronize() do
          @read_conversion_cache.clear
        end
      end
    end

    # Sets the hazardous description of the packet
    #
    # @param hazardous_description [String] Hazardous description of the packet
    def hazardous_description=(hazardous_description)
      if hazardous_description
        raise ArgumentError, "hazardous_description must be a String but is a #{hazardous_description.class}" unless String === hazardous_description
        @hazardous_description = hazardous_description.clone.freeze
      else
        @hazardous_description = nil
      end
    end

    # Saves a hash of the values given by a user when constructing a command
    #
    # @param given_values [Hash<Item Name, Value>] Hash of given command parameters
    def given_values=(given_values)
      if given_values
        raise ArgumentError, "given_values must be a Hash but is a #{given_values.class}" unless Hash === given_values
        @given_values = given_values.clone
      else
        @given_values = nil
      end
    end

    # Sets the callback object called when a limits state changes
    #
    # @param limits_change_callback [#call] Object must respond to the call
    #   method and take the following arguments: packet (Packet), item (PacketItem),
    #   old_limits_state (Symbol), item_value (Object), log_change (Boolean). The
    #   current item state can be found by querying the item object:
    #   item.limits.state.
    def limits_change_callback=(limits_change_callback)
      if limits_change_callback
        raise ArgumentError, "limits_change_callback must respond to call" unless limits_change_callback.respond_to?(:call)
        @limits_change_callback = limits_change_callback
      else
        @limits_change_callback = nil
      end
    end

    # Review bit offset to look for overlapping definitions. This will allow
    # gaps in the packet, but not allow the same bits to be used for multiple
    # variables.
    #
    # @return [Array<String>] Warning messages for big definition overlaps
    def check_bit_offsets
      expected_next_offset = nil
      previous_item = nil
      warnings = []
      @sorted_items.each do |item|
        if expected_next_offset and item.bit_offset < expected_next_offset
          msg = "Bit definition overlap at bit offset #{item.bit_offset} for packet #{@target_name} #{@packet_name} items #{item.name} and #{previous_item.name}"
          Logger.instance.warn(msg)
          warnings << msg
        end
        expected_next_offset = Packet.next_bit_offset(item)
        previous_item = item
      end
      warnings
    end

    # Checks if the packet has any gaps or overlapped items
    #
    # @return [Boolean] true if the packet has no gaps or overlapped items
    def packed?
      expected_next_offset = nil
      @sorted_items.each do |item|
        if expected_next_offset and item.bit_offset != expected_next_offset
          return false
        end
        expected_next_offset = Packet.next_bit_offset(item)
      end
      true
    end

    # Returns the bit offset of the next item after the current item if items are packed
    #
    # @param item [PacketItem] The item to calculate the next offset for
    # @return [Integer] Bit Offset of Next Item if Packed
    def self.next_bit_offset(item)
      if item.array_size
        if item.array_size > 0
          next_offset = item.bit_offset + item.array_size
        else
          next_offset = item.array_size
        end
      else
        next_offset = nil
        if item.bit_offset > 0
          if item.little_endian_bit_field?
            # Bit offset always refers to the most significant bit of a bitfield
            bits_remaining_in_last_byte = 8 - (item.bit_offset % 8)
            if item.bit_size > bits_remaining_in_last_byte
              next_offset = item.bit_offset + bits_remaining_in_last_byte
            end
          end
        end
        unless next_offset
          if item.bit_size > 0
            next_offset = item.bit_offset + item.bit_size
          else
            next_offset = item.bit_size
          end
        end
      end
      next_offset
    end

    # Id items are used by the identify? method to determine if a raw buffer of
    # data represents this packet.
    # @return [Array<PacketItem>] Packet item identifiers
    def id_items
      @id_items ||= []
    end

    # @return [Array<PacketItem>] All items with defined limits
    def limits_items
      @limits_items ||= []
    end

    # @return [Hash] Hash of processors associated with this packet
    def processors
      @processors ||= {}
    end

    # Returns packet specific metadata
    # @return [Hash<Meta Name, Meta Values>]
    def meta
      @meta ||= {}
    end

    # Indicates if the packet has been identified
    # @return [TrueClass or FalseClass]
    def identified?
      !@target_name.nil? && !@packet_name.nil?
    end

    # Define an item in the packet. This creates a new instance of the
    # item_class as given in the constructor and adds it to the items hash. It
    # also resizes the buffer to accomodate the new item.
    #
    # @param name [String] Name of the item. Used by the items hash to retrieve
    #   the item.
    # @param bit_offset [Integer] Bit offset of the item in the raw buffer
    # @param bit_size [Integer] Bit size of the item in the raw buffer
    # @param data_type [Symbol] Type of data contained by the item. This is
    #   dependant on the item_class but by default see StructureItem.
    # @param array_size [Integer] Set to a non nil value if the item is to
    #   represented as an array.
    # @param endianness [Symbol] Endianness of this item. By default the
    #   endianness as set in the constructure is used.
    # @param overflow [Symbol] How to handle value overflows. This is
    #   dependant on the item_class but by default see StructureItem.
    # @param format_string [String] String to pass to Kernel#sprintf
    # @param read_conversion [Conversion] Conversion to apply when reading the
    #   item from the packet buffer
    # @param write_conversion [Conversion] Conversion to apply before writing
    #   the item to the packet buffer
    # @param id_value [Object] Set to something other than nil to indicate that
    #   this item should be used to identify a buffer as this packet. The
    #   id_value should make sense according to the data_type.
    # @return [PacketItem] The new packet item
    def define_item(name, bit_offset, bit_size, data_type, array_size = nil, endianness = @default_endianness, overflow = :ERROR, format_string = nil, read_conversion = nil, write_conversion = nil, id_value = nil)
      item = super(name, bit_offset, bit_size, data_type, array_size, endianness, overflow)
      packet_define_item(item, format_string, read_conversion, write_conversion, id_value)
    end

    # Add an item to the packet by adding it to the items hash. It also
    # resizes the buffer to accomodate the new item.
    #
    # @param item [PacketItem] Item to add to the packet
    # @return [PacketItem] The same packet item
    def define(item)
      item = super(item)
      update_id_items(item)
      update_limits_items_cache(item)
      item
    end

    # Define an item at the end of the packet. This creates a new instance of the
    # item_class as given in the constructor and adds it to the items hash. It
    # also resizes the buffer to accomodate the new item.
    #
    # @param name (see #define_item)
    # @param bit_size (see #define_item)
    # @param data_type (see #define_item)
    # @param array_size (see #define_item)
    # @param endianness (see #define_item)
    # @param overflow (see #define_item)
    # @param format_string (see #define_item)
    # @param read_conversion (see #define_item)
    # @param write_conversion (see #define_item)
    # @param id_value (see #define_item)
    # @return (see #define_item)
    def append_item(name, bit_size, data_type, array_size = nil, endianness = @default_endianness, overflow = :ERROR, format_string = nil, read_conversion = nil, write_conversion = nil, id_value = nil)
      item = super(name, bit_size, data_type, array_size, endianness, overflow)
      packet_define_item(item, format_string, read_conversion, write_conversion, id_value)
    end

    # (see Structure#get_item)
    def get_item(name)
      super(name)
    rescue ArgumentError
      raise "Packet item '#{@target_name} #{@packet_name} #{name.upcase}' does not exist"
    end

    # Read an item in the packet
    #
    # @param item [PacketItem] Instance of PacketItem or one of its subclasses
    # @param value_type [Symbol] How to convert the item before returning it.
    #   Must be one of {VALUE_TYPES}
    # @param buffer (see Structure#read_item)
    # @return The value. :FORMATTED and :WITH_UNITS values are always returned
    #   as Strings. :RAW values will match their data_type. :CONVERTED values
    #   can be any type.
    def read_item(item, value_type = :CONVERTED, buffer = @buffer)
      value = super(item, :RAW, buffer)
      derived_raw = false
      if item.data_type == :DERIVED && value_type == :RAW
        value_type = :CONVERTED
        derived_raw = true
      end
      case value_type
      when :RAW
        # Done above
      when :CONVERTED, :FORMATTED, :WITH_UNITS
        if item.read_conversion
          using_cached_value = false

          check_cache = buffer.equal?(@buffer)
          if check_cache and @read_conversion_cache
            synchronize_allow_reads() do
              if @read_conversion_cache[item]
                value = @read_conversion_cache[item]

                # Make sure cached value is not modified by anyone by creating
                # a deep copy
                if String === value
                  value = value.clone
                elsif Array === value
                  value = Marshal.load(Marshal.dump(value))
                end

                using_cached_value = true
              end
            end
          end

          unless using_cached_value
            if item.array_size
              value.map! do |val, index|
                item.read_conversion.call(val, self, buffer)
              end
            else
              value = item.read_conversion.call(value, self, buffer)
            end
            if check_cache
              synchronize_allow_reads() do
                @read_conversion_cache ||= {}
                @read_conversion_cache[item] = value

                # Make sure cached value is not modified by anyone by creating
                # a deep copy
                if String === value
                  value = value.clone
                elsif Array === value
                  value = Marshal.load(Marshal.dump(value))
                end
              end
            end
          end
        end

        # Derived raw values perform read_conversions but nothing else
        return value if derived_raw

        # Convert from value to state if possible
        if item.states
          if Array === value
            value = value.map do |val, index|
              if item.states.key(val)
                item.states.key(val)
              elsif item.states.values.include?(CATCH_ALL_STATE)
                item.states.key(CATCH_ALL_STATE)
              else
                apply_format_string_and_units(item, val, value_type)
              end
            end
          else
            state_value = item.states.key(value)
            if state_value
              value = state_value
            elsif item.states.values.include?(CATCH_ALL_STATE)
              value = item.states.key(CATCH_ALL_STATE)
            else
              value = apply_format_string_and_units(item, value, value_type)
            end
          end
        else
          if Array === value
            value = value.map do |val, index|
              apply_format_string_and_units(item, val, value_type)
            end
          else
            value = apply_format_string_and_units(item, value, value_type)
          end
        end
      else
        raise ArgumentError, "Unknown value type on read: #{value_type}"
      end
      return value
    end

    # Write an item in the packet
    #
    # @param item [PacketItem] Instance of PacketItem or one of its subclasses
    # @param value (see Structure#write_item)
    # @param value_type (see #read_item)
    # @param buffer (see Structure#write_item)
    def write_item(item, value, value_type = :CONVERTED, buffer = @buffer)
      case value_type
      when :RAW
        super(item, value, value_type, buffer)
      when :CONVERTED
        if item.states
          # Convert from state to value if possible
          state_value = item.states[value.to_s.upcase]
          value = state_value if state_value
        end
        if item.write_conversion
          value = item.write_conversion.call(value, self, buffer)
        else
          raise "Cannot write DERIVED item without a write conversion" if item.data_type == :DERIVED
        end
        begin
          super(item, value, :RAW, buffer) unless item.data_type == :DERIVED
        rescue ArgumentError => err
          if item.states and String === value and err.message =~ /invalid value for/
            raise "Unknown state #{value} for #{item.name}"
          else
            raise err
          end
        end
      when :FORMATTED, :WITH_UNITS
        raise ArgumentError, "Invalid value type on write: #{value_type}"
      else
        raise ArgumentError, "Unknown value type on write: #{value_type}"
      end
      if @read_conversion_cache
        synchronize() do
          @read_conversion_cache.clear
        end
      end
    end

    # Read an item in the packet by name
    #
    # @param name [String] Name of the item to read
    # @param value_type (see #read_item)
    # @param buffer (see #read_item)
    # @return (see #read_item)
    def read(name, value_type = :CONVERTED, buffer = @buffer)
      return super(name, value_type, buffer)
    end

    # Write an item in the packet by name
    #
    # @param name [String] Name of the item to write
    # @param value (see #write_item)
    # @param value_type (see #write_item)
    # @param buffer (see #write_item)
    def write(name, value, value_type = :CONVERTED, buffer = @buffer)
      super(name, value, value_type, buffer)
    end

    # Read all items in the packet into an array of arrays
    #   [[item name, item value], ...]
    #
    # @param value_type (see #read_item)
    # @param buffer (see Structure#read_all)
    # @param top (See Structure#read_all)
    # @return (see Structure#read_all)
    def read_all(value_type = :CONVERTED, buffer = @buffer, top = true)
      return super(value_type, buffer, top)
    end

    # Read all items in the packet into an array of arrays
    #   [[item name, item value], [item limits state], ...]
    #
    # @param value_type (see #read_all)
    # @param buffer (see #read_all)
    # @return [Array<String, Object, Symbol|nil>] Returns an Array consisting
    #   of [item name, item value, item limits state] where the item limits
    #   state can be one of {Cosmos::Limits::LIMITS_STATES}
    def read_all_with_limits_states(value_type = :CONVERTED, buffer = @buffer)
      result = nil
      synchronize_allow_reads(true) do
        result = read_all(value_type, buffer, false).map! do |array|
          array << @items[array[0]].limits.state
        end
      end
      return result
    end

    # Create a string that shows the name and value of each item in the packet
    #
    # @param value_type (see #read_item)
    # @param indent (see Structure#formatted)
    # @param buffer (see Structure#formatted)
    # @return (see Structure#formatted)
    def formatted(value_type = :CONVERTED, indent = 0, buffer = @buffer)
      return super(value_type, indent, buffer)
    end

    # Restore all items in the packet to their default value
    #
    # @param buffer [String] Raw buffer of binary data
    # @param skip_item_names [Array] Array of item names to skip
    def restore_defaults(buffer = @buffer, skip_item_names = nil)
      upcase_skip_item_names = skip_item_names.map(&:upcase) if skip_item_names
      @sorted_items.each do |item|
        write_item(item, item.default, :CONVERTED, buffer) unless (skip_item_names and upcase_skip_item_names.include?(item.name))
      end
    end

    # Define the reserved items on the current telemetry packet
    def define_reserved_items
      item = define_item('PACKET_TIMESECONDS', 0, 0, :DERIVED, nil, @default_endianness,
                         :ERROR, '%0.6f', PacketTimeSecondsConversion.new)
      item.description = 'COSMOS Packet Time (UTC, Floating point, Unix epoch)'
      item = define_item('PACKET_TIMEFORMATTED', 0, 0, :DERIVED, nil, @default_endianness,
                         :ERROR, nil, PacketTimeFormattedConversion.new)
      item.description = 'COSMOS Packet Time (Local time zone, Formatted string)'
      item = define_item('RECEIVED_TIMESECONDS', 0, 0, :DERIVED, nil, @default_endianness,
                         :ERROR, '%0.6f', ReceivedTimeSecondsConversion.new)
      item.description = 'COSMOS Received Time (UTC, Floating point, Unix epoch)'
      item = define_item('RECEIVED_TIMEFORMATTED', 0, 0, :DERIVED, nil, @default_endianness,
                         :ERROR, nil, ReceivedTimeFormattedConversion.new)
      item.description = 'COSMOS Received Time (Local time zone, Formatted string)'
      item = define_item('RECEIVED_COUNT', 0, 0, :DERIVED, nil, @default_endianness,
                         :ERROR, nil, ReceivedCountConversion.new)
      item.description = 'COSMOS packet received count'
    end

    # Enable limits on an item by name
    #
    # @param name [String] Name of the item to enable limits
    def enable_limits(name)
      get_item(name).limits.enabled = true
    end

    # Disable limits on an item by name
    #
    # @param name [String] Name of the item to disable limits
    def disable_limits(name)
      item = get_item(name)
      item.limits.enabled = false
      unless item.limits.state == :STALE
        old_limits_state = item.limits.state
        item.limits.state = nil
        @limits_change_callback.call(self, item, old_limits_state, nil, false) if @limits_change_callback
      end
    end

    # Add an item to the limits items cache if necessary.
    # You MUST call this after adding limits to an item
    #This is an optimization so we don't have to iterate through all the items when
    # checking for limits.
    def update_limits_items_cache(item)
      if item.limits.values || item.state_colors
        @limits_items ||= []
        @limits_items_hash ||= {}
        unless @limits_items_hash[item]
          @limits_items << item
          @limits_items_hash[item] = true
        end
      end
    end

    # Return an array of arrays indicating all items in the packet that are out of limits
    #   [[target name, packet name, item name, item limits state], ...]
    #
    # @return [Array<Array<String, String, String, Symbol>>]
    def out_of_limits
      items = []
      return items unless @limits_items
      @limits_items.each do |item|
        if (item.limits.enabled && item.limits.state &&
          PacketItemLimits::OUT_OF_LIMITS_STATES.include?(item.limits.state))
          items << [@target_name, @packet_name, item.name, item.limits.state]
        end
      end
      return items
    end

    # Set the limits state for all items to the given state
    #
    # @param state [Symbol] Must be one of PacketItemLimits::LIMITS_STATES
    def set_all_limits_states(state)
      @sorted_items.each {|item| item.limits.state = state}
    end

    # Check all the items in the packet against their defined limits. Update
    # their internal limits state and persistence and call the
    # limits_change_callback as necessary.
    #
    # @param limits_set [Symbol] Which limits set to check the item values
    #   against.
    # @param ignore_persistence [Boolean] Whether to ignore persistence when
    #   checking for out of limits
    def check_limits(limits_set = :DEFAULT, ignore_persistence = false)
      # If check_limits is being called, then a new packet has arrived and
      # this packet is no longer stale
      # Stored telemetry doesn't affect the current value table and such doesn't affect stale
      if @stale and !@stored
        @stale = false
        set_all_limits_states(nil)
      end

      return unless @limits_items
      @limits_items.each do |item|
        # Verify limits monitoring is enabled for this item
        if item.limits.enabled
          value = read_item(item)

          # Handle state monitoring and value monitoring differently
          if item.states
            handle_limits_states(item, value)
          elsif item.limits.values
            handle_limits_values(item, value, limits_set, ignore_persistence)
          end
        end
      end
    end

    # Sets the overall packet stale state to true and sets each packet item
    # limits state to :STALE.
    def set_stale
      @stale = true
      set_all_limits_states(:STALE)
    end

    # Reset temporary packet data
    # This includes packet received time, received count, and processor state
    def reset
      # The SYSTEM META packet is a special case that does not get reset
      return if @target_name == 'SYSTEM' && @packet_name == 'META'

      @received_time = nil
      @received_count = 0
      @stored = false
      @extra = nil
      if @read_conversion_cache
        synchronize() do
          @read_conversion_cache.clear
        end
      end
      return unless @processors
      @processors.each do |processor_name, processor|
        processor.reset
      end
    end

    # Make a light weight clone of this packet. This only creates a new buffer
    # of data and clones the processors. The defined packet items are the same.
    #
    # @return [Packet] A copy of the current packet with a new underlying
    #   buffer of data and processors
    def clone
      packet = super()
      if packet.instance_variable_get("@processors".freeze)
        packet.instance_variable_set("@processors".freeze, packet.processors.clone)
        packet.processors.each do |processor_name, processor|
          packet.processors[processor_name] = processor.clone
        end
      end
      packet.instance_variable_set("@read_conversion_cache".freeze, nil)
      packet.extra = JSON.parse(packet.extra.to_json) if packet.extra # Deep copy using JSON
      packet
    end
    alias dup clone

    def update_id_items(item)
      if item.id_value
        @id_items ||= []
        # Add to Id Items
        unless @id_items.empty?
          last_item = @id_items[-1]
          @id_items << item
          # If the current item or last item have a negative offset then we have
          # to re-sort. We also re-sort if the current item is less than the last
          # item because we are inserting.
          if last_item.bit_offset <= 0 or item.bit_offset <= 0 or item.bit_offset < last_item.bit_offset
            @id_items = @id_items.sort
          end
        else
          @id_items << item
        end
      end
      item
    end

    def to_config(cmd_or_tlm)
      config = ''

      if cmd_or_tlm == :TELEMETRY
        config << "TELEMETRY #{self.target_name.to_s.quote_if_necessary} #{self.packet_name.to_s.quote_if_necessary} #{@default_endianness} \"#{self.description}\"\n"
      else
        config << "COMMAND #{self.target_name.to_s.quote_if_necessary} #{self.packet_name.to_s.quote_if_necessary} #{@default_endianness} \"#{self.description}\"\n"
      end
      config << "  ALLOW_SHORT\n" if self.short_buffer_allowed
      config << "  HAZARDOUS #{self.hazardous_description.to_s.quote_if_necessary}\n" if self.hazardous
      config << "  DISABLE_MESSAGES\n" if self.messages_disabled
      if self.disabled
        config << "  DISABLED\n"
      elsif self.hidden
        config << "  HIDDEN\n"
      end

      if @processors
        @processors.each do |processor_name, processor|
          config << processor.to_config
        end
      end

      if @meta
        @meta.each do |key, values|
          config << "  META #{key.to_s.quote_if_necessary} #{values.map {|a| a..to_s.quote_if_necessary}.join(" ")}\n"
        end
      end

      # Items with derived items last
      self.sorted_items.each do |item|
        if item.data_type != :DERIVED
          config << item.to_config(cmd_or_tlm, @default_endianness)
        end
      end
      self.sorted_items.each do |item|
        if item.data_type == :DERIVED
          unless RESERVED_ITEM_NAMES.include?(item.name)
            config << item.to_config(cmd_or_tlm, @default_endianness)
          end
        end
      end

      config
    end

    protected

    # Performs packet specific processing on the packet.
    # Intended to only be run once for each packet received
    def process(buffer = @buffer)
      return unless @processors
      @processors.each do |processor_name, processor|
        processor.call(self, buffer)
      end
    end

    def handle_limits_states(item, value)
      # Retrieve limits state for the given value
      limits_state = item.state_colors[value]

      if item.limits.state != limits_state # PacketItemLimits state has changed
        # Save old limits state
        old_limits_state = item.limits.state
        # Update to new limits state
        item.limits.state = limits_state

        if old_limits_state == nil # Changing from nil
          if limits_state != :GREEN && limits_state != :BLUE # Warnings are needed
            @limits_change_callback.call(self, item, old_limits_state, value, true) if @limits_change_callback
          end
        else # Changing from a state other than nil so always call the callback
          if @limits_change_callback
            if item.limits.state.nil?
              @limits_change_callback.call(self, item, old_limits_state, value, false)
            else
              @limits_change_callback.call(self, item, old_limits_state, value, true)
            end
          end
        end
      end
    end

    def handle_limits_values(item, value, limits_set, ignore_persistence)
      # Retrieve limits settings for the specified limits_set
      limits = item.limits.values[limits_set]

      # Use the default limits set if limits aren't specified for the
      # particular limits set
      limits = item.limits.values[:DEFAULT] unless limits

      # Extract limits from array
      red_low     = limits[0]
      yellow_low  = limits[1]
      yellow_high = limits[2]
      red_high    = limits[3]
      green_low   = limits[4]
      green_high  = limits[5]
      limits_state = nil

      # Determine the limits_state based on the limits values and the current
      # value of the item
      if (value > yellow_low)
        if (value < yellow_high)
          if green_low
            if value < green_high
              if value > green_low
                limits_state = :BLUE
              else
                limits_state = :GREEN_LOW
              end
            else
              limits_state = :GREEN_HIGH
            end
          else
            limits_state = :GREEN
          end
        elsif (value < red_high)
          limits_state = :YELLOW_HIGH
        else
          limits_state = :RED_HIGH
        end
      else # value <= yellow_low
        if (value > red_low)
          limits_state = :YELLOW_LOW
        else
          limits_state = :RED_LOW
        end
      end

      if item.limits.state != limits_state # limits state has changed
        # Save old limits state for use in the callback
        old_limits_state = item.limits.state

        item.limits.persistence_count += 1

        # Check for item to achieve its persistence which means we
        # have to update the state and call the callback
        # Note when going back to green (or blue) persistence is ignored
        if (item.limits.persistence_count >= item.limits.persistence_setting) || ignore_persistence
          item.limits.state = limits_state

          # Additional actions for limits change
          @limits_change_callback.call(self, item, old_limits_state, value, true) if @limits_change_callback

          # Clear persistence since we've entered a new state
          item.limits.persistence_count = 0
        end
      else # limits state has not changed so clear persistence
        item.limits.persistence_count = 0
      end
    end

    def apply_format_string_and_units(item, value, value_type)
      if value_type == :FORMATTED or value_type == :WITH_UNITS
        if item.format_string && value
          value = sprintf(item.format_string, value)
        else
          value = value.to_s
        end
      end
      value << ' ' << item.units if value_type == :WITH_UNITS and item.units
      value
    end

    def packet_define_item(item, format_string, read_conversion, write_conversion, id_value)
      item.format_string = format_string
      item.read_conversion = read_conversion
      item.write_conversion = write_conversion

      # Change id_value to the correct type
      if id_value
        item.id_value = id_value
        update_id_items(item)
      end
      item
    end
  end
end
